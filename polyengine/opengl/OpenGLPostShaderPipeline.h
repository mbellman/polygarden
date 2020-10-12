#pragma once

#include <vector>

#include "opengl/AbstractOpenGLPostShader.h"
#include "opengl/OpenGLScreenQuad.h"
#include "subsystem/Math.h"

class OpenGLPostShaderPipeline {
public:
  ~OpenGLPostShaderPipeline();

  void addPostShader(AbstractOpenGLPostShader* glPostShader);
  void createFrameBuffers(const Area<unsigned int>& size);
  AbstractOpenGLPostShader* getFirstShader();
  void render();

private:
  std::vector<AbstractOpenGLPostShader*> glPostShaders;
};