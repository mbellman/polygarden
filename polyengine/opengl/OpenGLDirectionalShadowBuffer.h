#pragma once

#include "opengl/AbstractBuffer.h"

class OpenGLDirectionalShadowBuffer : public AbstractBuffer {
public:
  void createFrameBuffer(unsigned int width, unsigned int height) override;
  void writeToShadowCascade(unsigned int cascadeIndex);
};