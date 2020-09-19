#pragma once

#include "opengl/OpenGLVideoController.h"
#include "opengl/OpenGLShadowCaster.h"
#include "opengl/OpenGLLightingQuad.h"

class OpenGLIlluminator {
public:
  OpenGLIlluminator();
  ~OpenGLIlluminator();

  void renderIlluminatedSurfaces();
  void renderShadowCasters();
  void setVideoController(OpenGLVideoController* glVideoController);

private:
  OpenGLVideoController* glVideoController = nullptr;
  OpenGLLightingQuad* glLightingQuad = nullptr;

  bool isObjectWithinLightRadius(const Object* object, const Light* light);
  void renderDirectionalShadowCaster(OpenGLShadowCaster* glShadowCaster);
  void renderPointShadowCaster(OpenGLShadowCaster* glShadowCaster);
  void renderSpotShadowCaster(OpenGLShadowCaster* glShadowCaster);
};