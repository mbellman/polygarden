#pragma once

#include <vector>

#include "subsystem/entities/Light.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/Math.h"
#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLObject.h"

class OpenGLShadowCaster {
public:
  OpenGLShadowCaster(const Light* light);

  const Light* getLight() const;
  Matrix4 getCascadedLightMatrix(int cascadeIndex, const Camera& camera) const;
  Matrix4 getLightMatrix(const Vec3f& direction, const Vec3f& top) const;
  void startCasting();
  void startReading();

private:
  static const float cascadeSizes[3][2];

  const Light* light = nullptr;
};