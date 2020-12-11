#include <algorithm>
#include <vector>

#include "opengl/OpenGLIlluminator.h"
#include "opengl/OpenGLDebugger.h"
#include "opengl/OpenGLScreenQuad.h"
#include "opengl/OpenGLDirectionalShadowBuffer.h"
#include "opengl/OpenGLSpotShadowBuffer.h"
#include "opengl/OpenGLPointShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/PerformanceProfiler.h"

static bool isActiveDirectionalShadowCaster(const OpenGLShadowCaster* glShadowCaster) {
  return (
    glShadowCaster->getSourceLight()->type == Light::LightType::DIRECTIONAL &&
    glShadowCaster->getSourceLight()->power > 0.0f
  );
};

static bool isActiveSpotShadowCaster(const OpenGLShadowCaster* glShadowCaster) {
  return (
    glShadowCaster->getSourceLight()->type == Light::LightType::SPOTLIGHT &&
    glShadowCaster->getSourceLight()->power > 0.0f
  );
};

static bool isActivePointShadowCaster(const OpenGLShadowCaster* glShadowCaster) {
  auto* light = glShadowCaster->getSourceLight();

  return (
    light->type == Light::LightType::POINT &&
    light->power > 0.0f && (
      light->getLocalPosition().z > 0.0f ||
      light->getLocalDistance() < light->radius * 0.5f
    )
  );
};

static bool isObjectWithinLightRadius(const Object* object, const Light* light) {
  return (
    std::abs(object->position.x - light->position.x) < light->radius &&
    std::abs(object->position.y - light->position.y) < light->radius &&
    std::abs(object->position.z - light->position.z) < light->radius
  );
}

OpenGLIlluminator::OpenGLIlluminator() {
  glLightingQuad = new OpenGLLightingQuad();

  createShaderPrograms();
}

OpenGLIlluminator::~OpenGLIlluminator() {
  delete glLightingQuad;
}

void OpenGLIlluminator::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/lightview.fragment.glsl"));
  lightViewProgram.link();

  pointLightViewProgram.create();
  pointLightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/point-lightview.vertex.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadGeometryShader("./shaders/point-lightview.geometry.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/point-lightview.fragment.glsl"));
  pointLightViewProgram.link();

  directionalCameraViewProgram.create();
  directionalCameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  directionalCameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/directional-shadowcaster.fragment.glsl"));
  directionalCameraViewProgram.link();

  spotCameraViewProgram.create();
  spotCameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  spotCameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/spot-shadowcaster.fragment.glsl"));
  spotCameraViewProgram.link();

  pointCameraViewProgram.create();
  pointCameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  pointCameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/point-shadowcaster.fragment.glsl"));
  pointCameraViewProgram.link();
}

void OpenGLIlluminator::renderNonShadowCasterLights() {
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

void OpenGLIlluminator::renderShadowCasterLights() {
  auto& glShadowCasters = glVideoController->glShadowCasters;
  std::vector<OpenGLShadowCaster*> directionalShadowCasters;
  std::vector<OpenGLShadowCaster*> spotShadowCasters;
  std::vector<OpenGLShadowCaster*> pointShadowCasters;

  for (auto* glShadowCaster : glShadowCasters) {
    if (isActiveDirectionalShadowCaster(glShadowCaster)) {
      directionalShadowCasters.push_back(glShadowCaster);
    } else if (isActiveSpotShadowCaster(glShadowCaster)) {
      spotShadowCasters.push_back(glShadowCaster);
    } else if (isActivePointShadowCaster(glShadowCaster)) {
      pointShadowCasters.push_back(glShadowCaster);
    }
  }

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Render directional light shadow maps first, since these don't
  // need to dynamically enable/disable objects based on their
  // proximity to the light. Instead we defer to the existing
  // object enabled/disabled states, which can be determined
  // in game logic rather than engine logic.
  if (directionalShadowCasters.size() > 0 || spotShadowCasters.size() > 0) {  
    lightViewProgram.use();
  }

  for (auto* glShadowCaster : directionalShadowCasters) {
    renderDirectionalShadowCasterLightView(glShadowCaster);
  }

  // Render spot/point lights next, since these dynamically update
  // object enabled/disabled states based on proximity.
  for (auto* glShadowCaster : spotShadowCasters) {
    renderSpotShadowCasterLightView(glShadowCaster);
  }

  // Whenever there are multiple active point shadowcasters, render
  // their light views on a rotating basis - the active one determined
  // by the current frame - to reduce per-frame rendering work. This
  // may result in a reduced apparent "shadow framerate" if too many
  // are grouped together in close proximity.
  //
  // TODO: Allow point lights to override the active index check and
  // update their shadow map every frame.
  unsigned int cycleIndex = 0;

  unsigned int activePointShadowCasterIndex = pointShadowCasters.size() > 0
    ? (PerformanceProfiler::getCurrentFrame() % pointShadowCasters.size())
    : 0;

  if (pointShadowCasters.size() > 0) {
    pointLightViewProgram.use();
  }

  for (auto* glShadowCaster : pointShadowCasters) {
    if (cycleIndex++ == activePointShadowCasterIndex) {
      renderPointShadowCasterLightView(glShadowCaster);
    }
  }

  // After the shadow maps are drawn, render the lights with shadow
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);

  if (directionalShadowCasters.size() > 0) {
    directionalCameraViewProgram.use();
  }

  for (auto* glShadowCaster : directionalShadowCasters) {
    renderDirectionalShadowCasterCameraView(glShadowCaster);
  }

  if (spotShadowCasters.size() > 0) {
    spotCameraViewProgram.use();
  }

  for (auto* glShadowCaster : spotShadowCasters) {
    renderSpotShadowCasterCameraView(glShadowCaster);
  }

  if (pointShadowCasters.size() > 0) {
    pointCameraViewProgram.use();
  }

  for (auto* glShadowCaster : pointShadowCasters) {
    renderPointShadowCasterCameraView(glShadowCaster);
  }

  glDisable(GL_BLEND);

  // Tentatively re-enable all objects for rendering. No objects will
  // actually be rendered again until after the next game tick, which
  // can allow enabled/disabled states to be changed once more before
  // objects are rehydrated in Stage::update().
  for (auto* glObject : glVideoController->glObjects) {
    glObject->getSourceObject()->enableRenderingAll();
  }
}

void OpenGLIlluminator::renderDirectionalShadowCasterCameraView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLDirectionalShadowBuffer>();
  auto* light = glShadowCaster->getSourceLight();

  Matrix4 lightMatrixCascades[] = {
    glShadowCaster->getCascadedLightMatrix(0, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(1, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(2, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(3, *Camera::active)
  };

  directionalCameraViewProgram.use();
  directionalCameraViewProgram.setInt("colorTexture", 0);
  directionalCameraViewProgram.setInt("normalDepthTexture", 1);
  directionalCameraViewProgram.setInt("positionTexture", 2);
  directionalCameraViewProgram.setInt("lightMaps[0]", 3);
  directionalCameraViewProgram.setInt("lightMaps[1]", 4);
  directionalCameraViewProgram.setInt("lightMaps[2]", 5);
  directionalCameraViewProgram.setInt("lightMaps[3]", 6);
  directionalCameraViewProgram.setMatrix4("lightMatrixCascades[0]", lightMatrixCascades[0]);
  directionalCameraViewProgram.setMatrix4("lightMatrixCascades[1]", lightMatrixCascades[1]);
  directionalCameraViewProgram.setMatrix4("lightMatrixCascades[2]", lightMatrixCascades[2]);
  directionalCameraViewProgram.setMatrix4("lightMatrixCascades[3]", lightMatrixCascades[3]);
  directionalCameraViewProgram.setVec3f("cameraPosition", Camera::active->position);
  directionalCameraViewProgram.setVec3f("light.position", light->position);
  directionalCameraViewProgram.setVec3f("light.direction", light->direction.unit());
  directionalCameraViewProgram.setVec3f("light.color", light->color * light->power);
  directionalCameraViewProgram.setFloat("light.radius", light->radius);
  directionalCameraViewProgram.setInt("light.type", light->type);

  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glVideoController->gBuffer->startReading();
  glShadowBuffer->startReading();

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::renderDirectionalShadowCasterLightView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLDirectionalShadowBuffer>();

  Matrix4 lightMatrixCascades[] = {
    glShadowCaster->getCascadedLightMatrix(0, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(1, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(2, *Camera::active),
    glShadowCaster->getCascadedLightMatrix(3, *Camera::active)
  };

  lightViewProgram.setInt("modelTexture", 7);
  glShadowBuffer->startWriting();

  for (int i = 0; i < 4; i++) {
    lightViewProgram.setMatrix4("lightMatrix", lightMatrixCascades[i]);
    glShadowBuffer->writeToShadowCascade(i);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* glObject : glVideoController->glObjects) {
      if (glObject->getSourceObject()->shadowCascadeLimit > i) {
        lightViewProgram.setBool("hasTexture", glObject->hasTexture());

        glVideoController->setObjectEffects(lightViewProgram, glObject);

        if (glObject->getSourceObject()->shadowLod != nullptr) {
          glObject->renderShadowLod();
        } else {
          glObject->render();
        }
      }
    }
  }
}

void OpenGLIlluminator::renderPointShadowCasterCameraView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLPointShadowBuffer>();
  auto* light = glShadowCaster->getSourceLight();

  pointCameraViewProgram.setInt("colorTexture", 0);
  pointCameraViewProgram.setInt("normalDepthTexture", 1);
  pointCameraViewProgram.setInt("positionTexture", 2);
  pointCameraViewProgram.setInt("lightCubeMap", 3);
  pointCameraViewProgram.setFloat("farPlane", light->radius);
  pointCameraViewProgram.setVec3f("cameraPosition", Camera::active->position);
  pointCameraViewProgram.setVec3f("light.position", light->position);
  pointCameraViewProgram.setVec3f("light.direction", light->direction);
  pointCameraViewProgram.setVec3f("light.color", light->color * light->power);
  pointCameraViewProgram.setFloat("light.radius", light->radius);
  pointCameraViewProgram.setInt("light.type", light->type);

  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glVideoController->gBuffer->startReading();
  glShadowBuffer->startReading();

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::renderPointShadowCasterLightView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLPointShadowBuffer>();
  auto* light = glShadowCaster->getSourceLight();

  Matrix4 lightMatrices[6] = {
    glShadowCaster->getLightMatrix(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(-1.0f, 0.0f, 0.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0.0f, 0.0f, -1.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 0.0f, -1.0f), Vec3f(0.0f, -1.0f, 0.0f)),
    glShadowCaster->getLightMatrix(Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, -1.0f, 0.0f))
  };

  pointLightViewProgram.use();
  pointLightViewProgram.setVec3f("lightPosition", light->position.gl());
  pointLightViewProgram.setFloat("farPlane", light->radius);

  for (int i = 0; i < 6; i++) {
    pointLightViewProgram.setMatrix4("lightMatrices[" + std::to_string(i) + "]", lightMatrices[i]);
  }

  glShadowBuffer->startWriting();

  glClear(GL_DEPTH_BUFFER_BIT);

  for (auto* glObject : glVideoController->glObjects) {
    auto* sourceObject = glObject->getSourceObject();

    if (sourceObject->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(pointLightViewProgram, glObject);

      // TODO: Allow objects to force point lights to render them
      // anyway, e.g. large objects with origins further away from the
      // light source than their radius, but geometry within the radius
      sourceObject->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getSourceLight());
      });

      sourceObject->rehydrate();

      if (sourceObject->shadowLod != nullptr) {
        glObject->renderShadowLod();
      } else {
        glObject->render();
      }
    }
  }
}

void OpenGLIlluminator::renderSpotShadowCasterCameraView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLSpotShadowBuffer>();
  auto* light = glShadowCaster->getSourceLight();
  Matrix4 lightMatrix = glShadowCaster->getLightMatrix(light->direction, Vec3f(0.0f, 1.0f, 0.0f));

  glVideoController->glPostShaderPipeline->getFirstShader()->writeToInputBuffer();
  glVideoController->gBuffer->startReading();
  glShadowBuffer->startReading();

  spotCameraViewProgram.use();
  spotCameraViewProgram.setInt("colorTexture", 0);
  spotCameraViewProgram.setInt("normalDepthTexture", 1);
  spotCameraViewProgram.setInt("positionTexture", 2);
  spotCameraViewProgram.setInt("lightMap", 3);
  spotCameraViewProgram.setMatrix4("lightMatrix", lightMatrix);
  spotCameraViewProgram.setVec3f("cameraPosition", Camera::active->position);
  spotCameraViewProgram.setVec3f("light.position", light->position);
  spotCameraViewProgram.setVec3f("light.direction", light->direction.unit());
  spotCameraViewProgram.setVec3f("light.color", light->color * light->power);
  spotCameraViewProgram.setFloat("light.radius", light->radius);
  spotCameraViewProgram.setInt("light.type", light->type);

  OpenGLScreenQuad::draw();
  PerformanceProfiler::trackLight(light);
}

void OpenGLIlluminator::renderSpotShadowCasterLightView(OpenGLShadowCaster* glShadowCaster) {
  auto* glShadowBuffer = glShadowCaster->getShadowBuffer<OpenGLSpotShadowBuffer>();
  Matrix4 lightMatrix = glShadowCaster->getLightMatrix(glShadowCaster->getSourceLight()->direction, Vec3f(0.0f, 1.0f, 0.0f));

  lightViewProgram.setMatrix4("lightMatrix", lightMatrix);
  glShadowBuffer->startWriting();

  glClear(GL_DEPTH_BUFFER_BIT);

  for (auto* glObject : glVideoController->glObjects) {
    auto* sourceObject = glObject->getSourceObject();

    if (sourceObject->shadowCascadeLimit > 0) {
      glVideoController->setObjectEffects(lightViewProgram, glObject);

      // TODO: Allow objects to force spot lights to render them
      // anyway, e.g. large objects with origins further away from the
      // light source than their radius, but geometry within the radius
      sourceObject->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getSourceLight());
      });

      sourceObject->rehydrate();

      if (sourceObject->shadowLod != nullptr) {
        glObject->renderShadowLod();
      } else {
        glObject->render();
      }
    }
  }
}

void OpenGLIlluminator::setVideoController(OpenGLVideoController* glVideoController) {
  this->glVideoController = glVideoController;
}