#include <algorithm>
#include <vector>

#include "opengl/OpenGLIlluminator.h"
#include "opengl/OpenGLDebugger.h"
#include "opengl/OpenGLScreenQuad.h"
#include "opengl/OpenGLDirectionalShadowBuffer.h"
#include "opengl/OpenGLSpotShadowBuffer.h"
#include "opengl/OpenGLPointShadowBuffer.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/PerformanceProfiler.h"

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
    if (
      glShadowCaster->getSourceLight()->type == Light::LightType::DIRECTIONAL &&
      glShadowCaster->getSourceLight()->power > 0.0f
    ) {
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
    if (glShadowCaster->getSourceLight()->power > 0.0f) {
      switch (glShadowCaster->getSourceLight()->type) {
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
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLDirectionalShadowBuffer>();
  auto& lightViewProgram = glShadowBuffer->getLightViewProgram();
  auto& cameraViewProgram = glShadowBuffer->getCameraViewProgram();

  lightViewProgram.use();
  lightViewProgram.setInt("modelTexture", 7);

  glShadowBuffer->startWriting();

  const Camera& camera = glVideoController->scene->getCamera();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Matrix4 lightMatrixCascades[] = {
    glShadowCaster->getCascadedLightMatrix(0, camera),
    glShadowCaster->getCascadedLightMatrix(1, camera),
    glShadowCaster->getCascadedLightMatrix(2, camera),
    glShadowCaster->getCascadedLightMatrix(3, camera)
  };

  for (int i = 0; i < 4; i++) {
    glShadowBuffer->writeToShadowCascade(i);
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

  auto* light = glShadowCaster->getSourceLight();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glShadowBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  cameraViewProgram.use();
  cameraViewProgram.setInt("colorTexture", 0);
  cameraViewProgram.setInt("normalDepthTexture", 1);
  cameraViewProgram.setInt("positionTexture", 2);
  cameraViewProgram.setInt("lightMaps[0]", 3);
  cameraViewProgram.setInt("lightMaps[1]", 4);
  cameraViewProgram.setInt("lightMaps[2]", 5);
  cameraViewProgram.setInt("lightMaps[3]", 6);
  cameraViewProgram.setMatrix4("lightMatrixCascades[0]", lightMatrixCascades[0]);
  cameraViewProgram.setMatrix4("lightMatrixCascades[1]", lightMatrixCascades[1]);
  cameraViewProgram.setMatrix4("lightMatrixCascades[2]", lightMatrixCascades[2]);
  cameraViewProgram.setMatrix4("lightMatrixCascades[3]", lightMatrixCascades[3]);
  cameraViewProgram.setVec3f("cameraPosition", camera.position);
  cameraViewProgram.setVec3f("light.position", light->position);
  cameraViewProgram.setVec3f("light.direction", light->direction.unit());
  cameraViewProgram.setVec3f("light.color", light->color * light->power);
  cameraViewProgram.setFloat("light.radius", light->radius);
  cameraViewProgram.setInt("light.type", light->type);

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::renderPointShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto* light = glShadowCaster->getSourceLight();

  if (light->getLocalPosition().z < 0.0f && light->getLocalDistance() > light->radius * 0.5f) {
    return;
  }

  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLPointShadowBuffer>();
  auto& lightViewProgram = glShadowBuffer->getLightViewProgram();
  auto& cameraViewProgram = glShadowBuffer->getCameraViewProgram();

  // Point light view pass
  lightViewProgram.use();
  glShadowBuffer->startWriting();

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

  lightViewProgram.setVec3f("lightPosition", light->position.gl());
  lightViewProgram.setFloat("farPlane", light->radius + 1000.0f);

  for (int i = 0; i < 6; i++) {
    lightViewProgram.setMatrix4("lightMatrices[" + std::to_string(i) + "]", lightMatrices[i]);
  }

  for (auto* glObject : glVideoController->glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(lightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getSourceLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glShadowBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  cameraViewProgram.use();
  cameraViewProgram.setInt("colorTexture", 0);
  cameraViewProgram.setInt("normalDepthTexture", 1);
  cameraViewProgram.setInt("positionTexture", 2);
  cameraViewProgram.setInt("lightCubeMap", 3);
  cameraViewProgram.setFloat("farPlane", light->radius + 1000.0f);
  cameraViewProgram.setVec3f("cameraPosition", camera.position);
  cameraViewProgram.setVec3f("light.position", light->position);
  cameraViewProgram.setVec3f("light.direction", light->direction);
  cameraViewProgram.setVec3f("light.color", light->color * light->power);
  cameraViewProgram.setFloat("light.radius", light->radius);
  cameraViewProgram.setInt("light.type", light->type);

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::renderSpotShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLSpotShadowBuffer>();
  auto& lightViewProgram = glShadowBuffer->getLightViewProgram();
  auto& cameraViewProgram = glShadowBuffer->getCameraViewProgram();

  // Light view pass
  lightViewProgram.use();
  glShadowBuffer->startWriting();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  auto* light = glShadowCaster->getSourceLight();
  Matrix4 lightMatrix = glShadowCaster->getLightMatrix(light->direction, Vec3f(0.0f, 1.0f, 0.0f));

  glShadowBuffer->startWriting();
  lightViewProgram.setMatrix4("lightMatrix", lightMatrix);

  for (auto* glObject : glVideoController->glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(lightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getSourceLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  const Camera& camera = glVideoController->scene->getCamera();

  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glShadowBuffer->startReading();
  glVideoController->gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  cameraViewProgram.use();
  cameraViewProgram.setInt("colorTexture", 0);
  cameraViewProgram.setInt("normalDepthTexture", 1);
  cameraViewProgram.setInt("positionTexture", 2);
  cameraViewProgram.setInt("lightMap", 3);
  cameraViewProgram.setMatrix4("lightMatrix", lightMatrix);
  cameraViewProgram.setVec3f("cameraPosition", camera.position);
  cameraViewProgram.setVec3f("light.position", light->position);
  cameraViewProgram.setVec3f("light.direction", light->direction.unit());
  cameraViewProgram.setVec3f("light.color", light->color * light->power);
  cameraViewProgram.setFloat("light.radius", light->radius);
  cameraViewProgram.setInt("light.type", light->type);

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::setVideoController(OpenGLVideoController* glVideoController) {
  this->glVideoController = glVideoController;
}