#pragma once

#include "opengl/AbstractBuffer.h"

class OpenGLPointShadowBuffer : public AbstractBuffer {
public:
  void createFrameBuffer(unsigned int width, unsigned int height) override;
};