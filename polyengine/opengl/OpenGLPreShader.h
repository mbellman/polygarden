#pragma once

#include "opengl/ShaderProgram.h"
#include "opengl/OpenGLScreenQuad.h"

class OpenGLPreShader {
public:
  OpenGLPreShader(const char* path);
  ~OpenGLPreShader();

  ShaderProgram& getProgram();
  void render();

private:
  ShaderProgram program;
  OpenGLScreenQuad* glScreenQuad = nullptr;
};