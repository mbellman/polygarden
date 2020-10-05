#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"
#include "opengl/FrameBuffer.h"

class OpenGLSpotShadowBuffer : public AbstractBuffer {
public:
  OpenGLSpotShadowBuffer();

  void createFrameBuffer(unsigned int width, unsigned int height) override;
  ShaderProgram& getCameraViewProgram();
  ShaderProgram& getLightViewProgram();

protected:
  void createShaderPrograms() override;

private:
  ShaderProgram lightViewProgram;
  ShaderProgram cameraViewProgram;
};