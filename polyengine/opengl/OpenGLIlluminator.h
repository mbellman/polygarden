#pragma once

#include "opengl/OpenGLVideoController.h"
#include "opengl/OpenGLShadowCaster.h"

class OpenGLIlluminator {
public:
  void renderIlluminatedSurfaces();
  void renderShadowCasters();
  void setVideoController(OpenGLVideoController* glVideoController);

private:
  OpenGLVideoController* glVideoController = nullptr;

  bool isObjectWithinLightRadius(const Object* object, const Light* light);
  void renderDirectionalShadowCaster(OpenGLShadowCaster* glShadowCaster);
  void renderPointShadowCaster(OpenGLShadowCaster* glShadowCaster);
  void renderSpotShadowCaster(OpenGLShadowCaster* glShadowCaster);
};