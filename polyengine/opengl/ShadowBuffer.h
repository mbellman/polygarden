#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"
#include "opengl/FrameBuffer.h"

class ShadowBuffer : public AbstractBuffer {
public:
  ShadowBuffer();

  void blur();
  void createFrameBuffer(unsigned int width, unsigned int height) override;
  ShaderProgram& getDirectionalShadowProgram();
  ShaderProgram& getLightViewProgram();
  ShaderProgram& getSpotShadowProgram();
  void writeToShadowCascade(unsigned int cascadeIndex);

protected:
  void createShaderPrograms() override;

private:
  ShaderProgram lightViewProgram;
  ShaderProgram directionalShadowProgram;
  ShaderProgram spotShadowProgram;
  ShaderProgram blurProgram;
  FrameBuffer* downsample_x = nullptr;
  FrameBuffer* downsample_y = nullptr;
};