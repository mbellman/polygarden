#pragma once

#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLScreenQuad.h"

class AbstractBuffer {
public:
  virtual ~AbstractBuffer();

  virtual void createFrameBuffer(unsigned int width, unsigned int height) = 0;
  FrameBuffer* getFrameBuffer();
  virtual void renderScreenQuad() final;
  virtual void startReading() final;
  virtual void startWriting() final;

protected:
  FrameBuffer* frameBuffer = nullptr;
  OpenGLScreenQuad* glScreenQuad = nullptr;

  virtual void createShaderPrograms() = 0;
};