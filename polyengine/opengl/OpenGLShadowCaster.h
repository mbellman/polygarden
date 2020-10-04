#pragma once

#include <vector>

#include "subsystem/entities/Light.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/Math.h"
#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLObject.h"
#include "opengl/AbstractBuffer.h"

class OpenGLShadowCaster {
public:
  OpenGLShadowCaster(const Light* light);
  ~OpenGLShadowCaster();

  const Light* getSourceLight() const;
  Matrix4 getCascadedLightMatrix(int cascadeIndex, const Camera& camera) const;
  Matrix4 getLightMatrix(const Vec3f& direction, const Vec3f& top) const;

  template<typename T>
  T* getShadowBuffer() {
    return (T*)glShadowBuffer;
  }

private:
  static const float cascadeSizes[3][2];

  const Light* sourceLight = nullptr;
  AbstractBuffer* glShadowBuffer = nullptr;
};