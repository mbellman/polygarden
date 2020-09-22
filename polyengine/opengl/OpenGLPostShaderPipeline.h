#pragma once

#include <vector>

#include "opengl/AbstractOpenGLPostShader.h"
#include "opengl/OpenGLScreenQuad.h"
#include "subsystem/Math.h"

class OpenGLPostShaderPipeline {
public:
  OpenGLPostShaderPipeline();
  ~OpenGLPostShaderPipeline();

  void addPostShader(AbstractOpenGLPostShader* glPostShader);
  void createFrameBuffers(const Region2d<int>& screen);
  AbstractOpenGLPostShader* getFirstShader();
  void render();

private:
  std::vector<AbstractOpenGLPostShader*> glPostShaders;
  OpenGLScreenQuad* glScreenQuad = nullptr;
};