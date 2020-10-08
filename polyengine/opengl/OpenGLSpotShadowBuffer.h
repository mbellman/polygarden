#pragma once

#include "opengl/AbstractBuffer.h"

class OpenGLSpotShadowBuffer : public AbstractBuffer {
public:
  void createFrameBuffer(unsigned int width, unsigned int height) override;
};