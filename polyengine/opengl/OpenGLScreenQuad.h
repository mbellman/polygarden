#pragma once

#include "glew.h"
#include "glut.h"

class OpenGLScreenQuad {
public:
  static void draw();

private:
  GLuint vao;
  GLuint vbo;
  static OpenGLScreenQuad* singleton;

  OpenGLScreenQuad();
  ~OpenGLScreenQuad();

  void render();
};