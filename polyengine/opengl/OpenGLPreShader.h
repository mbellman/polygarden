#pragma once

#include "opengl/ShaderProgram.h"

class OpenGLPreShader {
public:
  OpenGLPreShader(const char* path);
  ~OpenGLPreShader();

  ShaderProgram& getProgram();
  void render();

private:
  ShaderProgram program;
};