#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"

class SBuffer : public AbstractBuffer {
public:
  SBuffer();

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
};