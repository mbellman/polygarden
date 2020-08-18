#pragma once

#include "opengl/AbstractBuffer.h"
#include "opengl/ShaderProgram.h"
#include "opengl/OpenGLPipeline.h"

class SBuffer : public AbstractBuffer {
public:
  SBuffer();

  void createFrameBuffer(unsigned int width, unsigned int height) override;
  ShaderProgram& getDirectionalShadowProgram();
  ShaderProgram& getLightViewProgram();
  ShaderProgram& getPointLightViewProgram();
  ShaderProgram& getPointShadowProgram();
  ShaderProgram& getSpotShadowProgram();
  void writeToShadowCascade(unsigned int cascadeIndex);

protected:
  void createShaderPrograms() override;

private:
  ShaderProgram lightViewProgram;
  ShaderProgram pointLightViewProgram;
  ShaderProgram directionalShadowProgram;
  ShaderProgram pointShadowProgram;
  ShaderProgram spotShadowProgram;
};