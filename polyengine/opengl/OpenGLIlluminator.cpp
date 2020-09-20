#include <algorithm>
#include <vector>

#include "opengl/OpenGLIlluminator.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/entities/Camera.h"
#include "opengl/OpenGLDebugger.h"

OpenGLIlluminator::OpenGLIlluminator() {
  glLightingQuad = new OpenGLLightingQuad();
}

OpenGLIlluminator::~OpenGLIlluminator() {
  delete glLightingQuad;
}

bool OpenGLIlluminator::isObjectWithinLightRadius(const Object* object, const Light* light) {
  return (
    std::abs(object->position.x - light->position.x) < light->radius &&
    std::abs(object->position.y - light->position.y) < light->radius &&
    std::abs(object->position.z - light->position.z) < light->radius
  );
}

void OpenGLIlluminator::renderIlluminatedSurfaces() {
  auto& illuminationProgram = glVideoController->gBuffer->getShaderProgram(GBuffer::Shader::ILLUMINATION);

  illuminationProgram.use();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glEnable(GL_BLEND);

  auto* scene = glVideoController->scene;
  auto& lights = scene->getStage().getLights();

  illuminationProgram.setInt("colorTexture", 0);
  illuminationProgram.setInt("normalDepthTexture", 1);
  illuminationProgram.setInt("positionTexture", 2);
  illuminationProgram.setVec3f("cameraPosition", scene->getCamera().position);

  std::vector<Light*> nonShadowCasterLights;

  for (auto* light : lights) {
    if (light->power > 0.0f && !light->canCastShadows) {
      nonShadowCasterLights.push_back(light);
    }
  }

  glLightingQuad->render(nonShadowCasterLights);

  glDisable(GL_BLEND);
}

void OpenGLIlluminator::renderShadowCasters() {
  // Render the effects of shadow-casting directional lights first,
  // since these don't need to dynamically enable/disable objects
  // based on their proximity to the light. Instead we defer to the
  // existing object enabled/disabled states, which can be determined
  // in game logic rather than engine logic.
  for (auto* glShadowCaster : glVideoController->glShadowCasters) {
    if (glShadowCaster->getLight()->type == Light::LightType::DIRECTIONAL && glShadowCaster->getLight()->power > 0.0f) {
      renderDirectionalShadowCaster(glShadowCaster);
    }
  }

  // Render spot/point lights next, since these dynamically update
  // object enabled/disabled states based on proximity.
  //
  // TODO: Allow objects to force spot/point lights to render them
  // anyway, e.g. large objects with origins further away from the
  // light source than their radius, but geometry within the radius
  for (auto* glShadowCaster : glVideoController->glShadowCasters) {
    if (glShadowCaster->getLight()->power > 0.0f) {
      switch (glShadowCaster->getLight()->type) {
        case Light::LightType::SPOTLIGHT:
          renderSpotShadowCaster(glShadowCaster);
          break;
        case Light::LightType::POINT:
          renderPointShadowCaster(glShadowCaster);
          break;
        default:
          break;
      }
    }
  }

  // Tentatively re-enable all objects for rendering. No objects will
  // actually be rendered again until after the next game tick, which
  // can allow enabled/disabled states to be changed once more before
  // objects are rehydrated in Stage::update().
  for (auto* glObject : glVideoController->glObjects) {
    glObject->getSourceObject()->enableRenderingAll();
  }

  glDisable(GL_BLEND);
}

void OpenGLIlluminator::renderDirectionalShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto& lightViewProgram = glVideoController->sBuffer->getLightViewProgram();
  auto& directionalShadowProgram = glVideoController->sBuffer->getDirectionalShadowProgram();

  lightViewProgram.use();
  lightViewProgram.setInt("modelTexture", 7);

  glVideoController->sBuffer->startWriting();

  const Camera& camera = glVideoController->scene->getCamera();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Matrix4 lightMatrixCascades[] = {
    glShadowCaster->getCascadedLightMatrix(0, camera),
    glShadowCaster->getCascadedLightMatrix(1, camera),
    glShadowCaster->getCascadedLightMatrix(2, camera)
  };

  for (int i = 0; i < 3; i++) {
    glVideoController->sBuffer->writeToShadowCascade(i);
    lightViewProgram.setMatrix4("lightMatrix", lightMatrixCascades[i]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* glObject : glVideoController->glObjects) {
      if (glObject->getSourceObject()->shadowCascadeLimit > i) {
        lightViewProgram.setBool("hasTexture", glObject->hasTexture());

        glVideoController->setObjectEffects(lightViewProgram, glObject);

        glObject->render();
      }
    }
  }

  // Camera view shadowcaster lighting pass
  auto* light = glShadowCaster->getLight();

  glVideoController->glPostShaders[0]->startWriting();
  glVideoController->sBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  directionalShadowProgram.use();
  directionalShadowProgram.setInt("colorTexture", 0);
  directionalShadowProgram.setInt("normalDepthTexture", 1);
  directionalShadowProgram.setInt("positionTexture", 2);
  directionalShadowProgram.setInt("lightMaps[0]", 3);
  directionalShadowProgram.setInt("lightMaps[1]", 4);
  directionalShadowProgram.setInt("lightMaps[2]", 5);
  directionalShadowProgram.setMatrix4("lightMatrixCascades[0]", lightMatrixCascades[0]);
  directionalShadowProgram.setMatrix4("lightMatrixCascades[1]", lightMatrixCascades[1]);
  directionalShadowProgram.setMatrix4("lightMatrixCascades[2]", lightMatrixCascades[2]);
  directionalShadowProgram.setVec3f("cameraPosition", camera.position);
  directionalShadowProgram.setVec3f("light.position", light->position);
  directionalShadowProgram.setVec3f("light.direction", light->direction);
  directionalShadowProgram.setVec3f("light.color", light->color * light->power);
  directionalShadowProgram.setFloat("light.radius", light->radius);
  directionalShadowProgram.setInt("light.type", light->type);

  glVideoController->sBuffer->renderScreenQuad();
}

void OpenGLIlluminator::renderPointShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto* light = glShadowCaster->getLight();

  if (light->getLocalPosition().z < 0.0f && light->getLocalDistance() > light->radius) {
    return;
  }

  auto& pointLightViewProgram = glVideoController->pointShadowBuffer->getPointLightViewProgram();
  auto& pointShadowProgram = glVideoController->pointShadowBuffer->getPointShadowProgram();

  // Point light view pass
  pointLightViewProgram.use();
  glVideoController->pointShadowBuffer->startWriting();

  const Camera& camera = glVideoController->scene->getCamera();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  Matrix4 lightMatrices[6] = {
    glShadowCaster->getLightMatrix(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 0.0f, -1.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, -1.0f, 0.0f))
  };

  pointLightViewProgram.setVec3f("lightPosition", light->position.gl());
  pointLightViewProgram.setFloat("farPlane", light->radius + 1000.0f);

  for (int i = 0; i < 6; i++) {
    pointLightViewProgram.setMatrix4("lightMatrices[" + std::to_string(i) + "]", lightMatrices[i]);
  }

  for (auto* glObject : glVideoController->glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(pointLightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  glVideoController->glPostShaders[0]->startWriting();
  glVideoController->pointShadowBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  pointShadowProgram.use();
  pointShadowProgram.setInt("colorTexture", 0);
  pointShadowProgram.setInt("normalDepthTexture", 1);
  pointShadowProgram.setInt("positionTexture", 2);
  pointShadowProgram.setInt("lightCubeMap", 3);
  pointShadowProgram.setFloat("farPlane", light->radius + 1000.0f);
  pointShadowProgram.setVec3f("cameraPosition", camera.position);
  pointShadowProgram.setVec3f("light.position", light->position);
  pointShadowProgram.setVec3f("light.direction", light->direction);
  pointShadowProgram.setVec3f("light.color", light->color * light->power);
  pointShadowProgram.setFloat("light.radius", light->radius);
  pointShadowProgram.setInt("light.type", light->type);

  glVideoController->pointShadowBuffer->renderScreenQuad();
}

void OpenGLIlluminator::renderSpotShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto& lightViewProgram = glVideoController->sBuffer->getLightViewProgram();
  auto& spotShadowProgram = glVideoController->sBuffer->getSpotShadowProgram();

  // Light view pass
  lightViewProgram.use();

  glVideoController->sBuffer->startWriting();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  auto* light = glShadowCaster->getLight();
  Matrix4 lightMatrix = glShadowCaster->getLightMatrix(light->direction, Vec3f(0.0f, 1.0f, 0.0f));

  glVideoController->sBuffer->writeToShadowCascade(0);
  lightViewProgram.setMatrix4("lightMatrix", lightMatrix);

  for (auto* glObject : glVideoController->glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(lightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  const Camera& camera = glVideoController->scene->getCamera();

  glVideoController->glPostShaders[0]->startWriting();
  glVideoController->sBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  spotShadowProgram.use();
  spotShadowProgram.setInt("colorTexture", 0);
  spotShadowProgram.setInt("normalDepthTexture", 1);
  spotShadowProgram.setInt("positionTexture", 2);
  spotShadowProgram.setInt("lightMap", 3);
  spotShadowProgram.setMatrix4("lightMatrix", lightMatrix);
  spotShadowProgram.setVec3f("cameraPosition", camera.position);
  spotShadowProgram.setVec3f("light.position", light->position);
  spotShadowProgram.setVec3f("light.direction", light->direction);
  spotShadowProgram.setVec3f("light.color", light->color * light->power);
  spotShadowProgram.setFloat("light.radius", light->radius);
  spotShadowProgram.setInt("light.type", light->type);

  glVideoController->sBuffer->renderScreenQuad();
}

void OpenGLIlluminator::setVideoController(OpenGLVideoController* glVideoController) {
  this->glVideoController = glVideoController;
}