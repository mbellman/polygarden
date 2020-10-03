#include "opengl/OpenGLScreenQuad.h"
#include "subsystem/PerformanceProfiler.h"

const float QUAD_DATA[] = {
  -1.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f
};

OpenGLScreenQuad::OpenGLScreenQuad() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), QUAD_DATA, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

OpenGLScreenQuad::~OpenGLScreenQuad() {
  // TODO
}

void OpenGLScreenQuad::draw() {
  if (OpenGLScreenQuad::singleton == nullptr) {
    OpenGLScreenQuad::singleton = new OpenGLScreenQuad();
  }

  OpenGLScreenQuad::singleton->render();
}

void OpenGLScreenQuad::render() {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  PerformanceProfiler::trackDrawCall();
}

OpenGLScreenQuad* OpenGLScreenQuad::singleton = nullptr;