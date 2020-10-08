#pragma once

#include "opengl/OpenGLVideoController.h"
#include "opengl/OpenGLShadowCaster.h"
#include "opengl/OpenGLLightingQuad.h"
#include "opengl/ShaderProgram.h"
#include "opengl/FrameBuffer.h"

class OpenGLIlluminator {
public:
  OpenGLIlluminator();
  ~OpenGLIlluminator();

  void renderNonShadowCasterLights();
  void renderShadowCasterLights();
  void setVideoController(OpenGLVideoController* glVideoController);

private:
  OpenGLVideoController* glVideoController = nullptr;
  OpenGLLightingQuad* glLightingQuad = nullptr;
  ShaderProgram lightViewProgram;
  ShaderProgram pointLightViewProgram;
  ShaderProgram directionalCameraViewProgram;
  ShaderProgram spotCameraViewProgram;
  ShaderProgram pointCameraViewProgram;

  void createShaderPrograms();
  void renderDirectionalShadowCasterCameraView(OpenGLShadowCaster* OpenGLShadowCaster);
  void renderDirectionalShadowCasterLightView(OpenGLShadowCaster* glShadowCaster);
  void renderPointShadowCasterCameraView(OpenGLShadowCaster* glShadowCaster);
  void renderPointShadowCasterLightView(OpenGLShadowCaster* glShadowCaster);
  void renderSpotShadowCasterCameraView(OpenGLShadowCaster* glShadowCaster);
  void renderSpotShadowCasterLightView(OpenGLShadowCaster* glShadowCaster);
};