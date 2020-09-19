#pragma once

#include "glew.h"
#include "glut.h"

class OpenGLScreenQuad {
public:
  OpenGLScreenQuad();
  ~OpenGLScreenQuad();

  void render();

private:
  GLuint vao;
  GLuint vbo;
};