#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/AbstractOpenGLPostShader.h"
#include "opengl/ShaderLoader.h"

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

void AbstractOpenGLPostShader::createFrameBuffer(const Region2d<int>& screen) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(screen.width, screen.height);

  onConfigureFrameBuffer();

  frameBuffer->bindColorTextures();
}

FrameBuffer* AbstractOpenGLPostShader::getFrameBuffer() const {
  return frameBuffer;
}

void AbstractOpenGLPostShader::render() {
  frameBuffer->startReading();
  
  glClear(GL_COLOR_BUFFER_BIT);

  onRender();
}

ShaderProgram* AbstractOpenGLPostShader::getShaderProgram(std::string name) {
  return programMap.at(name);
}

ShaderProgram* AbstractOpenGLPostShader::getShaderProgram() {
  return getShaderProgram("__main__");
}

void AbstractOpenGLPostShader::startWriting() {
  frameBuffer->startWriting();
}