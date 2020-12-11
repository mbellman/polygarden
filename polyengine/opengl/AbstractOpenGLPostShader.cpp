#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/AbstractOpenGLPostShader.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/Window.h"

AbstractOpenGLPostShader::~AbstractOpenGLPostShader() {
  delete frameBuffer;

  for (auto& [ key, program ] : programMap) {
    delete program;
  }

  programMap.clear();
}

void AbstractOpenGLPostShader::addShaderProgram(std::string name, const char* path) {
  auto* program = new ShaderProgram();

  program->create();
  program->attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  program->attachShader(ShaderLoader::loadFragmentShader(path));
  program->link();

  programMap[name] = program;
}

void AbstractOpenGLPostShader::addShaderProgram(const char* path) {
  addShaderProgram("__main__", path);
}

void AbstractOpenGLPostShader::createFrameBuffer(const Area<unsigned int>& size) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(size.width, size.height);

  onConfigureFrameBuffer();

  frameBuffer->bindColorTextures();
}

FrameBuffer* AbstractOpenGLPostShader::getFrameBuffer() const {
  return frameBuffer;
}

ShaderProgram* AbstractOpenGLPostShader::getShaderProgram(std::string name) {
  return programMap.at(name);
}

ShaderProgram* AbstractOpenGLPostShader::getShaderProgram() {
  return getShaderProgram("__main__");
}

void AbstractOpenGLPostShader::render() {
  frameBuffer->startReading();
  
  glClear(GL_COLOR_BUFFER_BIT);

  onRender();
}

void AbstractOpenGLPostShader::setNextShader(AbstractOpenGLPostShader* glPostShader) {
  nextShader = glPostShader;
}

void AbstractOpenGLPostShader::writeToInputBuffer() {
  frameBuffer->startWriting();
}

void AbstractOpenGLPostShader::writeToOutputBuffer() {
  if (nextShader != nullptr) {
    nextShader->writeToInputBuffer();
  } else {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::size.width, Window::size.height);
  }
}