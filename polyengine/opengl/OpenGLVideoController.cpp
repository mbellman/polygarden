#include <cmath>
#include <ctime>
#include <cstdio>

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
#include "subsystem/Math.h"
#include "subsystem/Entities.h"

OpenGLVideoController::OpenGLVideoController() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
}

OpenGLVideoController::~OpenGLVideoController() {
  // debug();

  screenShaders.free();
  glObjects.free();
  glShadowCasters.free();

  OpenGLObject::freeCachedResources();

  delete gBuffer;
  delete sBuffer;
  delete pointShadowBuffer;
}

void OpenGLVideoController::createScreenShaders() {
  auto* dofShader = new ScreenShader("./polyengine/shaders/dof.fragment.glsl");
  auto* preBloomShader = new ScreenShader("./polyengine/shaders/prebloom.fragment.glsl");
  auto* postBloomShaderH = new ScreenShader("./polyengine/shaders/postbloom-h.fragment.glsl");
  auto* postBloomShaderV = new ScreenShader("./polyengine/shaders/postbloom-v.fragment.glsl");

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

  // Post-bloom H
  postBloomShaderH->onCreateFrameBuffer([=](const ShaderProgram& program, auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Base color
    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Bright color
    buffer->bindColorTextures();

    return buffer;
  });

  postBloomShaderH->onRender([=](const ShaderProgram& program, OpenGLScreenQuad* glScreenQuad) {
    glClear(GL_COLOR_BUFFER_BIT);

    program.setInt("baseColor", 0);
    program.setInt("bloomColor", 1);

    glScreenQuad->render();
  });

  // Post-bloom V
  postBloomShaderV->onCreateFrameBuffer([=](const ShaderProgram& program, auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Base color
    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Bright color
    buffer->bindColorTextures();

    return buffer;
  });

  postBloomShaderV->onRender([=](const ShaderProgram& program, OpenGLScreenQuad* glScreenQuad) {
    glClear(GL_COLOR_BUFFER_BIT);

    program.setInt("baseColor", 0);
    program.setInt("bloomColor", 1);

    glScreenQuad->render();
  });

  dofShader->createFrameBuffer(screenSize);
  preBloomShader->createFrameBuffer(screenSize);
  postBloomShaderH->createFrameBuffer(screenSize);
  postBloomShaderV->createFrameBuffer(screenSize);

  screenShaders.push(dofShader);
  screenShaders.push(preBloomShader);
  screenShaders.push(postBloomShaderH);
  screenShaders.push(postBloomShaderV);
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

SDL_Window* OpenGLVideoController::createWindow(const char* title, Region2d<int> region) {
  return SDL_CreateWindow(title, region.x, region.y, region.width, region.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
}

void OpenGLVideoController::debug() {
  for (auto* glObject : glObjects) {
    auto* source = glObject->getSourceObject();

    printf("-------------\n");
    printf("Total instances: %d\n", source->getTotalInstances());
    printf("Total polygons: %d\n", source->getPolygons().size());
    printf("Matrix:\n");

    source->getMatrix().debug();

    auto* buffer = source->getMatrixBuffer();

    for (int i = 0; i < source->getTotalInstances() * 16; i++) {
      if (i % 4 == 0) {
        printf("\n");
      }

      if (i % 16 == 0) {
        printf("\n");
      }

      printf("%f, ", buffer[i]);
    }

    printf("\n-------------\n");
  }
}

void OpenGLVideoController::onDestroy() {
  SDL_GL_DeleteContext(glContext);
}

void OpenGLVideoController::onEntityAdded(Entity* entity) {
  if (entity->isOfType<Object>()) {
    if (!((Object*)entity)->isInstance()) {
      glObjects.push(new OpenGLObject((Object*)entity));
    }
  } else if (entity->isOfType<Light>() && ((Light*)entity)->canCastShadows) {
    glShadowCasters.push(new OpenGLShadowCaster((Light*)entity));
  }
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

void OpenGLVideoController::onInit() {
  glContext = SDL_GL_CreateContext(sdlWindow);
  glewExperimental = true;

  glewInit();

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glCullFace(GL_BACK);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  SDL_GL_SetSwapInterval(0);

  gBuffer = new GBuffer();
  sBuffer = new SBuffer();
  pointShadowBuffer = new PointShadowBuffer();

  gBuffer->createFrameBuffer(screenSize.width, screenSize.height);
  sBuffer->createFrameBuffer(2048, 2048);
  pointShadowBuffer->createFrameBuffer(1024, 1024);

  createScreenShaders();

  gBuffer->getFrameBuffer()->shareDepthStencilBuffer(screenShaders[0]->getFrameBuffer());

  scene->onEntityAdded([=](auto* entity) {
    onEntityAdded(entity);
  });

  scene->onEntityRemoved([=](auto* entity) {
    onEntityRemoved(entity);
  });

  scene->onInit();

  OpenGLDebugger::checkErrors("Scene Initialization");
}

void OpenGLVideoController::onRender() {
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
    glClear(GL_DEPTH_BUFFER_BIT);

    sBuffer->writeToShadowCascade(i);
    lightViewProgram.setMatrix4("lightMatrix", lightMatrixCascades[i]);

    for (auto* glObject : glObjects) {
      glObject->render();
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

  Matrix4 projectionMatrix = Matrix4::projection(screenSize, 45.0f, 1.0f, 10000.0f).transpose();
  Matrix4 viewMatrix = createViewMatrix();

  auto renderObject = [&](OpenGLObject* glObject) {
    auto& program = glObject->hasCustomShader()
      ? *glObject->getCustomShader()
      : geometryProgram;

    program.use();
    program.setMatrix4("projectionMatrix", projectionMatrix);
    program.setMatrix4("viewMatrix", viewMatrix);
    program.setBool("hasTexture", glObject->hasTexture());
    program.setBool("hasNormalMap", glObject->hasNormalMap());
    program.setInt("modelTexture", 7);
    program.setInt("normalMap", 8);
    program.setFloat("time", scene->getRunningTime());

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

  // TODO: Render lights in chunks to allow a unlimited number
  // of lights + loop unrolling within the lighting shader
  for (auto* light : lights) {
    if (!light->canCastShadows) {
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
    glObject->render();
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
  for (auto* glShadowCaster : glShadowCasters) {
    switch (glShadowCaster->getLight()->type) {
      case Light::LightType::DIRECTIONAL:
        renderDirectionalShadowCaster(glShadowCaster);
        break;
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
    glObject->render();
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