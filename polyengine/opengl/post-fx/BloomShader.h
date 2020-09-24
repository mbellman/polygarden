#pragma once

#include "opengl/AbstractOpenGLPostShader.h"
#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLScreenQuad.h"

class BloomShader : public AbstractOpenGLPostShader {
public:
  ~BloomShader();

  void onInit();
  void onConfigureFrameBuffer();
  void onRender();

private:
  FrameBuffer* downsample1 = nullptr;
  FrameBuffer* downsample2 = nullptr;
  FrameBuffer* downsample3_h = nullptr;
  FrameBuffer* downsample3_v = nullptr;
  OpenGLScreenQuad* glScreenQuad = nullptr;
};