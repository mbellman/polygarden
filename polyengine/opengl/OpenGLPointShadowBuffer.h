#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"

class OpenGLPointShadowBuffer : public AbstractBuffer {
public:
  OpenGLPointShadowBuffer();

  void createFrameBuffer(unsigned int width, unsigned int height) override;
  ShaderProgram& getCameraViewProgram();
  ShaderProgram& getLightViewProgram();

protected:
  void createShaderPrograms() override;

private:
  ShaderProgram lightViewProgram;
  ShaderProgram cameraViewProgram;
};