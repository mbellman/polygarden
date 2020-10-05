#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"
#include "opengl/FrameBuffer.h"

class OpenGLDirectionalShadowBuffer : public AbstractBuffer {
public:
  OpenGLDirectionalShadowBuffer();

  void createFrameBuffer(unsigned int width, unsigned int height) override;
  ShaderProgram& getCameraViewProgram();
  ShaderProgram& getLightViewProgram();
  void writeToShadowCascade(unsigned int cascadeIndex);

protected:
  void createShaderPrograms() override;

private:
  ShaderProgram lightViewProgram;
  ShaderProgram cameraViewProgram;
};