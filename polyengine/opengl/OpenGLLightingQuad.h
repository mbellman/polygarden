#pragma once

#include <vector>

#include "glew.h"
#include "glut.h"
#include "subsystem/entities/Light.h"
#include "subsystem/Math.h"

class OpenGLLightingQuad {
public:
  OpenGLLightingQuad();
  ~OpenGLLightingQuad();

  void render(const std::vector<Light*>& lights);

private:
  GLuint vao;
  GLuint buffers[3];

  void bufferData(const std::vector<Light*>& lights);
  void defineLightAttributes();
  void defineQuadTransformAttributes();
  void defineQuadVertexAttributes();
};