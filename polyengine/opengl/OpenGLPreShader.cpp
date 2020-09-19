#include "opengl/OpenGLPreShader.h"
#include "opengl/ShaderLoader.h"

OpenGLPreShader::OpenGLPreShader(const char* path) {
  program.create();
  program.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  program.attachShader(ShaderLoader::loadFragmentShader(path));
  program.link();

  glScreenQuad = new OpenGLScreenQuad();
}

OpenGLPreShader::~OpenGLPreShader() {
  // TODO
}

ShaderProgram& OpenGLPreShader::getProgram() {
  return program;
}

void OpenGLPreShader::render() {
  glScreenQuad->render();
}