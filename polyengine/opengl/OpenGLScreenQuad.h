#pragma once

#include "glew.h"
#include "glut.h"

class OpenGLScreenQuad {
public:
  OpenGLScreenQuad();
  ~OpenGLScreenQuad();

  void bindVBO();
  void render();

private:
  GLuint vao;
  GLuint vbo;
};