#include <cmath>

#include "opengl/OpenGLShadowCaster.h"
#include "opengl/FrameBuffer.h"
#include "opengl/OpenGLDirectionalShadowBuffer.h"
#include "opengl/OpenGLSpotShadowBuffer.h"
#include "opengl/OpenGLPointShadowBuffer.h"

const static Range<float> CASCADE_PARAMETERS[4][2] = {
  { 1.0f, 200.0f },
  { 200.0f, 500.0f },
  { 500.0f, 1250.0f },
  { 1250.0f, 2500.0f }
};

OpenGLShadowCaster::OpenGLShadowCaster(const Light* light) {
  sourceLight = light;

  switch (light->type) {
    case Light::LightType::DIRECTIONAL:
      glShadowBuffer = new OpenGLDirectionalShadowBuffer();

      glShadowBuffer->createFrameBuffer(2048, 2048);
      break;
    case Light::LightType::SPOTLIGHT:
      glShadowBuffer = new OpenGLSpotShadowBuffer();

      glShadowBuffer->createFrameBuffer(1024, 1024);
      break;
    case Light::LightType::POINT:
      glShadowBuffer = new OpenGLPointShadowBuffer();

      glShadowBuffer->createFrameBuffer(512, 512);
      break;
  }
}

OpenGLShadowCaster::~OpenGLShadowCaster() {
  delete glShadowBuffer;
}

const Light* OpenGLShadowCaster::getSourceLight() const {
  return sourceLight;
}

Matrix4 OpenGLShadowCaster::getCascadedLightMatrix(int cascadeIndex, const Camera& camera) const {
  const Range<float>& range = *CASCADE_PARAMETERS[cascadeIndex];

  float tanFov = tanf(0.5f * camera.fov * M_PI / 180.0f);
  float tanNear = tanFov * range.start;
  float tanFar = tanFov * range.end;
  Vec3f forward = camera.getDirection();
  Vec3f left = camera.getLeftDirection();
  Vec3f up = Vec3f::crossProduct(left, forward);
  Vec3f nearCenter = camera.position + forward * range.start;
  Vec3f farCenter = camera.position + forward * range.end;
  Vec3f frustumCenter = nearCenter + forward * (range.end - range.start) * 0.5f;
  Matrix4 lightTransform = Matrix4::lookAt(frustumCenter, sourceLight->direction, Vec3f(0.0f, 1.0f, 0.0f));

  Frustum frustum;

  frustum.near[0] = nearCenter + (left * tanNear) + (up * tanNear);
  frustum.near[1] = nearCenter - (left * tanNear) + (up * tanNear);
  frustum.near[2] = nearCenter + (left * tanNear) - (up * tanNear);
  frustum.near[3] = nearCenter - (left * tanNear) - (up * tanNear);

  frustum.far[0] = farCenter + (left * tanFar) + (up * tanFar);
  frustum.far[1] = farCenter - (left * tanFar) + (up * tanFar);
  frustum.far[2] = farCenter + (left * tanFar) - (up * tanFar);
  frustum.far[3] = farCenter - (left * tanFar) - (up * tanFar);

  frustum = lightTransform * frustum;

  Bounds3d bounds = frustum.getBounds();
  Matrix4 projection = Matrix4::orthographic(bounds.top, bounds.bottom, bounds.left, bounds.right, bounds.back - 1000.0f, bounds.front);
  Matrix4 view = Matrix4::lookAt(frustumCenter.gl(), sourceLight->direction.invert().gl(), Vec3f(0.0f, 1.0f, 0.0f));

  return (projection * view).transpose();
}

Matrix4 OpenGLShadowCaster::getLightMatrix(const Vec3f& direction, const Vec3f& top) const {
  Matrix4 projection = Matrix4::projection({ 0, 0, 1024, 1024 }, 90.0f, 1.0f, sourceLight->radius);
  Matrix4 view = Matrix4::lookAt(sourceLight->position.gl(), direction.invert().gl(), top);

  return (projection * view).transpose();
}
