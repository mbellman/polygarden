#pragma once

#include "glew.h"
#include "glut.h"
#include "subsystem/Entities.h"

class OpenGLPipeline {
public:
  OpenGLPipeline();

  void bind();
  void bindMBO();
  void bindVBO();
  void createFromObject(const Object* object);
  void createScreenQuad();
  void render(unsigned int instanceCount = 1);
  void sendMatrixBuffer(unsigned int size, float* matrixBuffer);

private:
  GLuint vao;
  GLuint vbo;
  GLuint mbo;
  int totalVertices = 0;
};