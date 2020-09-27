#include "opengl/OpenGLPreShader.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

OpenGLPreShader::OpenGLPreShader(const char* path) {
  program.create();
  program.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  program.attachShader(ShaderLoader::loadFragmentShader(path));
  program.link();
}

OpenGLPreShader::~OpenGLPreShader() {
  // TODO
}

ShaderProgram& OpenGLPreShader::getProgram() {
  return program;
}

void OpenGLPreShader::render() {
  OpenGLScreenQuad::draw();
}