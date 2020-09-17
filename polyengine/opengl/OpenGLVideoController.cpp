#include <cmath>
#include <ctime>
#include <cstdio>
#include <algorithm>

#include "SDL.h"
#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/OpenGLVideoController.h"
#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLScreenQuad.h"
#include "opengl/OpenGLDebugger.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "opengl/ScreenShader.h"
#include "opengl/FrameBuffer.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"
#include "subsystem/entities/Instance.h"
#include "subsystem/Math.h"

OpenGLVideoController::OpenGLVideoController() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
}

OpenGLVideoController::~OpenGLVideoController() {
  screenShaders.free();
  glObjects.free();
  glShadowCasters.free();

  OpenGLObject::freeCachedResources();

  delete gBuffer;
  delete sBuffer;
  delete pointShadowBuffer;
}

void OpenGLVideoController::createScreenShaders() {
  auto* dofShader = new ScreenShader("./shaders/dof.fragment.glsl");
  auto* preBloomShader = new ScreenShader("./shaders/prebloom.fragment.glsl");
  auto* postBloomShader = new ScreenShader("./shaders/postbloom.fragment.glsl");

  // Depth-of-field
  dofShader->onCreateFrameBuffer([=](const ShaderProgram& program, auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);   // (0) Color/depth
    buffer->bindColorTextures();

    return buffer;
  });

  dofShader->onRender([=](const ShaderProgram& program, OpenGLScreenQuad* glScreenQuad) {
    glClear(GL_COLOR_BUFFER_BIT);

    program.setInt("screen", 0);

    glScreenQuad->render();
  });

  // Pre-bloom
  preBloomShader->onCreateFrameBuffer([=](const ShaderProgram& program, auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Color
    buffer->bindColorTextures();

    return buffer;
  });

  preBloomShader->onRender([=](const ShaderProgram& program, OpenGLScreenQuad* glScreenQuad) {
    glClear(GL_COLOR_BUFFER_BIT);

    program.setInt("screen", 0);

    glScreenQuad->render();
  });

  // Post-bloom
  postBloomShader->onCreateFrameBuffer([=](const ShaderProgram& program, auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Base color
    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Bright color
    buffer->bindColorTextures();

    return buffer;
  });

  postBloomShader->onRender([=](const ShaderProgram& program, OpenGLScreenQuad* glScreenQuad) {
    glClear(GL_COLOR_BUFFER_BIT);

    program.setInt("baseColor", 0);
    program.setInt("bloomColor", 1);

    glScreenQuad->render();
  });

  dofShader->createFrameBuffer(screenSize);
  preBloomShader->createFrameBuffer(screenSize);
  postBloomShader->createFrameBuffer(screenSize);

  screenShaders.push(dofShader);
  screenShaders.push(preBloomShader);
  screenShaders.push(postBloomShader);
}

Matrix4 OpenGLVideoController::createViewMatrix() {
  const Camera& camera = scene->getCamera();
  Vec3f rotation = camera.orientation * Vec3f(-1.0f, 1.0f, 1.0f);
  Vec3f translation = camera.position.invert().gl();

  return (
    Matrix4::rotate(rotation) *
    Matrix4::translate(translation)
  ).transpose();
}

bool OpenGLVideoController::isObjectWithinLightRadius(const Object* object, const Light* light) {
  return (
    std::abs(object->position.x - light->position.x) < light->radius &&
    std::abs(object->position.y - light->position.y) < light->radius &&
    std::abs(object->position.z - light->position.z) < light->radius
  );
}

void OpenGLVideoController::onDestroy() {
  SDL_GL_DeleteContext(glContext);
}

void OpenGLVideoController::onEntityAdded(Entity* entity) {
  if (entity->isOfType<Object>() && !entity->isOfType<Instance>()) {
    glObjects.push(new OpenGLObject((Object*)entity));
  } else if (entity->isOfType<Light>() && ((Light*)entity)->canCastShadows) {
    glShadowCasters.push(new OpenGLShadowCaster((Light*)entity));
  }

  OpenGLDebugger::checkErrors("Entity Added");
}

void OpenGLVideoController::onEntityRemoved(Entity* entity) {
  // TODO: Delete removed glObjects/glShadowCasters
  // from memory, since HeapList no longer does
  if (entity->isOfType<Object>()) {
    glObjects.removeWhere([=](OpenGLObject* glObject) {
      return glObject->getSourceObject() == entity;
    });
  } else if (entity->isOfType<Light>() && ((Light*)entity)->canCastShadows) {
    glShadowCasters.removeWhere([=](OpenGLShadowCaster* glShadowCaster) {
      return glShadowCaster->getLight() == entity;
    });
  }
}

void OpenGLVideoController::onInit(SDL_Window* sdlWindow, int width, int height) {
  screenSize.width = width;
  screenSize.height = height;

  glContext = SDL_GL_CreateContext(sdlWindow);
  glewExperimental = true;

  glewInit();

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glCullFace(GL_BACK);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  SDL_GL_SetSwapInterval(0);

  gBuffer = new GBuffer();
  sBuffer = new SBuffer();
  pointShadowBuffer = new PointShadowBuffer();

  gBuffer->createFrameBuffer(screenSize.width, screenSize.height);
  sBuffer->createFrameBuffer(2048, 2048);
  pointShadowBuffer->createFrameBuffer(1024, 1024);

  createScreenShaders();

  gBuffer->getFrameBuffer()->shareDepthStencilBuffer(screenShaders[0]->getFrameBuffer());

  OpenGLDebugger::checkErrors("Initialization");
}

void OpenGLVideoController::onRender(SDL_Window* sdlWindow) {
  gBuffer->startWriting();
  gBuffer->writeToAllBuffers();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  renderGeometry();

  screenShaders[0]->startWriting();
  gBuffer->startReading();

  renderEmissiveSurfaces();
  renderIlluminatedSurfaces();
  renderShadowCasters();
  renderScreenShaders();

  glStencilMask(0xFF);

  SDL_GL_SwapWindow(sdlWindow);
  glFinish();

  OpenGLDebugger::checkErrors("onRender");
}

void OpenGLVideoController::onSceneChange(AbstractScene* scene) {
  glObjects.free();
  glShadowCasters.free();

  for (auto* object : scene->getStage().getObjects()) {
    onEntityAdded(object);
  }

  for (auto* light : scene->getStage().getLights()) {
    onEntityAdded(light);
  }

  scene->onEntityAdded([=](auto* entity) {
    onEntityAdded(entity);
  });

  scene->onEntityRemoved([=](auto* entity) {
    onEntityRemoved(entity);
  });
}

void OpenGLVideoController::onScreenSizeChange(int width, int height) {
  screenSize.width = width;
  screenSize.height = height;

  glViewport(0, 0, width, height);

  gBuffer->createFrameBuffer(width, height);

  for (auto* shader : screenShaders) {
    shader->createFrameBuffer({ 0, 0, width, height });
  }

  gBuffer->getFrameBuffer()->shareDepthStencilBuffer(screenShaders[0]->getFrameBuffer());
}

void OpenGLVideoController::renderDirectionalShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto& lightViewProgram = sBuffer->getLightViewProgram();
  auto& directionalShadowProgram = sBuffer->getDirectionalShadowProgram();

  lightViewProgram.use();
  lightViewProgram.setInt("modelTexture", 7);

  sBuffer->startWriting();

  const Camera& camera = scene->getCamera();

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
    sBuffer->writeToShadowCascade(i);
    lightViewProgram.setMatrix4("lightMatrix", lightMatrixCascades[i]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* glObject : glObjects) {
      if (glObject->getSourceObject()->shadowCascadeLimit > i) {
        lightViewProgram.setBool("hasTexture", glObject->hasTexture());

        setObjectEffects(lightViewProgram, glObject);

        glObject->render();
      }
    }
  }

  // Camera view shadowcaster lighting pass
  auto* light = glShadowCaster->getLight();

  screenShaders[0]->startWriting();
  sBuffer->startReading();
  gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);

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

  sBuffer->renderScreenQuad();
}

void OpenGLVideoController::renderEmissiveSurfaces() {
  auto& albedoProgram = gBuffer->getShaderProgram(GBuffer::Shader::ALBEDO);

  albedoProgram.use();
  albedoProgram.setInt("colorTexture", 0);
  albedoProgram.setInt("normalDepthTexture", 1);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

  gBuffer->renderScreenQuad();
}

void OpenGLVideoController::renderGeometry() {
  auto& geometryProgram = gBuffer->getShaderProgram(GBuffer::Shader::GEOMETRY);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  geometryProgram.use();
  geometryProgram.setInt("modelTexture", 7);
  geometryProgram.setInt("normalMap", 8);

  Matrix4 projectionMatrix = Matrix4::projection(screenSize, scene->getCamera().fov * 0.5f, 1.0f, 10000.0f).transpose();
  Matrix4 viewMatrix = createViewMatrix();

  auto renderObject = [&](OpenGLObject* glObject) {
    geometryProgram.setMatrix4("projectionMatrix", projectionMatrix);
    geometryProgram.setMatrix4("viewMatrix", viewMatrix);
    geometryProgram.setBool("hasTexture", glObject->hasTexture());
    geometryProgram.setBool("hasNormalMap", glObject->hasNormalMap());

    setObjectEffects(geometryProgram, glObject);

    glObject->render();
  };

  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  for (auto* glObject : glObjects) {
    if (glObject->getSourceObject()->isEmissive) {
      renderObject(glObject);
    }
  }

  glStencilMask(0xFF);

  for (auto* glObject : glObjects) {
    if (!glObject->getSourceObject()->isEmissive) {
      renderObject(glObject);
    }
  }

  glStencilMask(0x00);
}

void OpenGLVideoController::renderIlluminatedSurfaces() {
  auto& illuminationProgram = gBuffer->getShaderProgram(GBuffer::Shader::ILLUMINATION);

  illuminationProgram.use();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 0xFF);

  auto& lights = scene->getStage().getLights();
  int totalLights = lights.length() - scene->getStage().getStats().totalShadowCasters;

  illuminationProgram.setInt("colorTexture", 0);
  illuminationProgram.setInt("normalDepthTexture", 1);
  illuminationProgram.setInt("positionTexture", 2);
  illuminationProgram.setVec3f("cameraPosition", scene->getCamera().position);
  illuminationProgram.setInt("totalLights", totalLights);

  int index = 0;

  // TODO: Use instance-rendered lighting quad + buffered light structs
  for (auto* light : lights) {
    if (light->power > 0.0f && !light->canCastShadows) {
      std::string idx = std::to_string(index++);

      illuminationProgram.setVec3f("lights[" + idx + "].position", light->position);
      illuminationProgram.setVec3f("lights[" + idx + "].direction", light->direction);
      illuminationProgram.setVec3f("lights[" + idx + "].color", light->color * light->power);
      illuminationProgram.setFloat("lights[" + idx + "].radius", light->radius);
      illuminationProgram.setInt("lights[" + idx + "].type", light->type);
    }
  }

  gBuffer->renderScreenQuad();
}

void OpenGLVideoController::renderPointShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto& pointLightViewProgram = pointShadowBuffer->getPointLightViewProgram();
  auto& pointShadowProgram = pointShadowBuffer->getPointShadowProgram();

  // Point light view pass
  pointLightViewProgram.use();
  pointShadowBuffer->startWriting();

  const Camera& camera = scene->getCamera();
  auto* light = glShadowCaster->getLight();

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

  for (auto* glObject : glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      setObjectEffects(pointLightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  screenShaders[0]->startWriting();
  pointShadowBuffer->startReading();
  gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);

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

  pointShadowBuffer->renderScreenQuad();
}

void OpenGLVideoController::renderScreenShaders() {
  glDisable(GL_STENCIL_TEST);

  for (int i = 0; i < screenShaders.length(); i++) {
    bool isFinalShader = i == screenShaders.length() - 1;

    if (isFinalShader) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    } else {
      screenShaders[i + 1]->startWriting();
    }

    screenShaders[i]->render();
  }
}

void OpenGLVideoController::renderShadowCasters() {
  // Render the effects of shadow-casting directional lights first,
  // since these don't need to dynamically enable/disable objects
  // based on their proximity to the light. Instead we defer to the
  // existing object enabled/disabled states, which can be determined
  // in game logic rather than engine logic.
  for (auto* glShadowCaster : glShadowCasters) {
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
  for (auto* glShadowCaster : glShadowCasters) {
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
  for (auto* glObject : glObjects) {
    glObject->getSourceObject()->enableRenderingAll();
  }

  glDisable(GL_BLEND);
}

void OpenGLVideoController::renderSpotShadowCaster(OpenGLShadowCaster* glShadowCaster) {
  auto& lightViewProgram = sBuffer->getLightViewProgram();
  auto& spotShadowProgram = sBuffer->getSpotShadowProgram();

  // Light view pass
  lightViewProgram.use();

  sBuffer->startWriting();

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClear(GL_DEPTH_BUFFER_BIT);

  auto* light = glShadowCaster->getLight();
  Matrix4 lightMatrix = glShadowCaster->getLightMatrix(light->direction, Vec3f(0.0f, 1.0f, 0.0f));

  sBuffer->writeToShadowCascade(0);
  lightViewProgram.setMatrix4("lightMatrix", lightMatrix);

  for (auto* glObject : glObjects) {
    if (glObject->getSourceObject()->shadowCascadeLimit > 0) {
      setObjectEffects(lightViewProgram, glObject);

      glObject->getSourceObject()->enableRenderingWhere([&](Object* object) {
        return isObjectWithinLightRadius(object, glShadowCaster->getLight());
      });

      glObject->getSourceObject()->rehydrate();
      glObject->render();
    }
  }

  // Camera view shadowcaster lighting pass
  const Camera& camera = scene->getCamera();

  screenShaders[0]->startWriting();
  sBuffer->startReading();
  gBuffer->startReading();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);

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

  sBuffer->renderScreenQuad();
}

void OpenGLVideoController::setObjectEffects(ShaderProgram& program, OpenGLObject* glObject) {
  unsigned int effects = glObject->getSourceObject()->effects;

  program.setFloat("time", scene->getRunningTime());
  program.setInt("treeTransformFactor", (effects & ObjectEffects::TREE_ANIMATION) ? 1.0f : 0.0f);
  program.setInt("grassTransformFactor", (effects & ObjectEffects::GRASS_ANIMATION) ? 1.0f : 0.0f);
}