#include "opengl/OpenGLPostShader.h"
#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"

OpenGLPostShader::OpenGLPostShader(const char* path) {
  program.create();
  program.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  program.attachShader(ShaderLoader::loadFragmentShader(path));
  program.link();

  glScreenQuad = new OpenGLScreenQuad();
}

OpenGLPostShader::~OpenGLPostShader() {
  delete glScreenQuad;
  delete frameBuffer;
}

void OpenGLPostShader::createFrameBuffer(const Region2d<int>& screen) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  program.use();

  frameBuffer = frameBufferFactory(screen);
}

FrameBuffer* OpenGLPostShader::getFrameBuffer() const {
  return frameBuffer;
}

void OpenGLPostShader::onCreateFrameBuffer(FrameBufferFactory factory) {
  frameBufferFactory = factory;
}

void OpenGLPostShader::onRender(RenderHandler handler) {
  renderHandler = handler;
}

void OpenGLPostShader::render() {
  frameBuffer->startReading();
  program.use();

  glClear(GL_COLOR_BUFFER_BIT);

  renderHandler(program);

  glScreenQuad->render();
}

void OpenGLPostShader::startWriting() {
  frameBuffer->startWriting();
}