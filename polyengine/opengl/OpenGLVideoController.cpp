#include <cmath>
#include <ctime>
#include <cstdio>
#include <algorithm>

#include "SDL.h"
#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/OpenGLVideoController.h"
#include "opengl/OpenGLIlluminator.h"
#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLScreenQuad.h"
#include "opengl/OpenGLDebugger.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
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
  glPreShaders.free();
  glPostShaders.free();
  glObjects.free();
  glShadowCasters.free();

  OpenGLObject::freeCachedResources();

  delete gBuffer;
  delete sBuffer;
  delete pointShadowBuffer;
  delete glIlluminator;
}

void OpenGLVideoController::createPostShaders() {
  auto* dofShader = new OpenGLPostShader("./shaders/post-fx/dof.fragment.glsl");
  auto* antiAliasingShader = new OpenGLPostShader("./shaders/post-fx/anti-aliasing.fragment.glsl");
  auto* chromaticAberrationShader = new OpenGLPostShader("./shaders/post-fx/chromatic-aberration.fragment.glsl");

  // Depth-of-field
  dofShader->setFrameBufferFactory([](auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);   // (0) Color/depth
    buffer->bindColorTextures();

    return buffer;
  });

  dofShader->setRenderHandler([](const ShaderProgram& program) {
    program.setInt("screen", 0);
  });

  // Anti-aliasing
  antiAliasingShader->setFrameBufferFactory([](auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Color
    buffer->bindColorTextures();

    return buffer;
  });

  antiAliasingShader->setRenderHandler([](const ShaderProgram& program) {
    program.setInt("screen", 0);
  });

  // Chromatic aberration
  chromaticAberrationShader->setFrameBufferFactory([](auto screen) {
    auto* buffer = new FrameBuffer(screen.width, screen.height);

    buffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE);   // (0) Color
    buffer->bindColorTextures();

    return buffer;
  });

  chromaticAberrationShader->setRenderHandler([](const ShaderProgram& program) {
    program.setInt("screen", 0);
  });

  dofShader->createFrameBuffer(screenSize);
  antiAliasingShader->createFrameBuffer(screenSize);
  chromaticAberrationShader->createFrameBuffer(screenSize);

  glPostShaders.push(dofShader);
  glPostShaders.push(antiAliasingShader);
  glPostShaders.push(chromaticAberrationShader);
}

void OpenGLVideoController::createPreShaders() {
  glPreShaders.push(new OpenGLPreShader("./shaders/pre-fx/fog.fragment.glsl"));
}

Matrix4 OpenGLVideoController::createViewMatrix() {
  const Camera& camera = scene->getCamera();

  return (
    Matrix4::rotate(camera.orientation) *
    Matrix4::translate(camera.position.invert().gl())
  ).transpose();
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
  glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  SDL_GL_SetSwapInterval(0);

  gBuffer = new GBuffer();
  sBuffer = new SBuffer();
  pointShadowBuffer = new PointShadowBuffer();
  glIlluminator = new OpenGLIlluminator();

  gBuffer->createFrameBuffer(screenSize.width, screenSize.height);
  sBuffer->createFrameBuffer(2048, 2048);
  pointShadowBuffer->createFrameBuffer(1024, 1024);
  glIlluminator->setVideoController(this);

  createPreShaders();
  createPostShaders();

  gBuffer->getFrameBuffer()->shareDepthStencilBuffer(glPostShaders[0]->getFrameBuffer());

  OpenGLDebugger::checkErrors("Initialization");
}

void OpenGLVideoController::onRender(SDL_Window* sdlWindow) {
  gBuffer->startWriting();
  gBuffer->writeToAllBuffers();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  renderGeometry();

  glPostShaders[0]->startWriting();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  gBuffer->startReading();

  renderEmissiveSurfaces();
  glIlluminator->renderIlluminatedSurfaces();
  glIlluminator->renderShadowCasters();
  renderPreShaders();
  renderPostShaders();

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

  for (auto* shader : glPostShaders) {
    shader->createFrameBuffer({ 0, 0, width, height });
  }

  gBuffer->getFrameBuffer()->shareDepthStencilBuffer(glPostShaders[0]->getFrameBuffer());
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

void OpenGLVideoController::renderPreShaders() {
  glEnable(GL_BLEND);

  for (auto* shader : glPreShaders) {
    shader->getProgram().use();
    shader->getProgram().setInt("colorTexture", 0);
    shader->getProgram().setInt("normalDepthTexture", 1);
    shader->getProgram().setInt("positionTexture", 2);
    shader->render();
  }

  glDisable(GL_BLEND);
}

void OpenGLVideoController::renderPostShaders() {
  glDisable(GL_STENCIL_TEST);

  for (int i = 0; i < glPostShaders.length(); i++) {
    bool isFinalShader = i == glPostShaders.length() - 1;

    if (isFinalShader) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    } else {
      glPostShaders[i + 1]->startWriting();
    }

    glPostShaders[i]->render();
  }
}

void OpenGLVideoController::setObjectEffects(ShaderProgram& program, OpenGLObject* glObject) {
  unsigned int effects = glObject->getSourceObject()->effects;

  program.setFloat("time", scene->getRunningTime());
  program.setInt("treeTransformFactor", (effects & ObjectEffects::TREE_ANIMATION) ? 1.0f : 0.0f);
  program.setInt("grassTransformFactor", (effects & ObjectEffects::GRASS_ANIMATION) ? 1.0f : 0.0f);
}