#include "glew.h"
#include "glut.h"
#include "opengl/OpenGLPipeline.h"

OpenGLPipeline::OpenGLPipeline() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &mbo);

  bind();
}

void OpenGLPipeline::bind() {
  glBindVertexArray(vao);
}

void OpenGLPipeline::bindMBO() {
  glBindBuffer(GL_ARRAY_BUFFER, mbo);
}

void OpenGLPipeline::bindVBO() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void OpenGLPipeline::createFromObject(const Object* object) {
  totalVertices = object->getPolygons().size() * 3;

  int bufferSize = totalVertices * 11;
  float* buffer = new float[bufferSize];
  int idx = 0;

  for (auto* polygon : object->getPolygons()) {
    for (int v = 0; v < 3; v++) {
      const Vertex3d& vertex = *polygon->vertices[v];

      buffer[idx++] = vertex.position.x;
      buffer[idx++] = vertex.position.y;
      buffer[idx++] = vertex.position.z;

      buffer[idx++] = vertex.normal.x;
      buffer[idx++] = vertex.normal.y;
      buffer[idx++] = vertex.normal.z;

      buffer[idx++] = vertex.tangent.x;
      buffer[idx++] = vertex.tangent.y;
      buffer[idx++] = vertex.tangent.z;

      buffer[idx++] = vertex.uv.x;
      buffer[idx++] = vertex.uv.y;
    }
  }

  bindVBO();
  glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), buffer, GL_STATIC_DRAW);

  delete[] buffer;
}

void OpenGLPipeline::createScreenQuad() {
  float quad[] = {
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f
  };

  bindVBO();
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), quad, GL_STATIC_DRAW);

  totalVertices = 6;
}

void OpenGLPipeline::render(unsigned int instanceCount) {
  bind();

  glDrawArraysInstanced(GL_TRIANGLES, 0, totalVertices, instanceCount);
}

void OpenGLPipeline::sendMatrixBuffer(unsigned int size, float* matrixBuffer) {
  bindMBO();
  glBufferData(GL_ARRAY_BUFFER, size, matrixBuffer, GL_DYNAMIC_DRAW);
}