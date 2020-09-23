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
  FrameBuffer* blurLevel1 = nullptr;
  FrameBuffer* blurLevel2 = nullptr;
  FrameBuffer* blurLevel3 = nullptr;
  OpenGLScreenQuad* glScreenQuad = nullptr;
};