#pragma once

#include "opengl/AbstractOpenGLPostShader.h"

class DofShader : public AbstractOpenGLPostShader {
public:
  void onInit() override;
  void onConfigureFrameBuffer() override;
  void onRender() override;
};