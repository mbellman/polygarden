#include "subsystem/entities/Camera.h"

constexpr static float PI = 3.141592f;
constexpr static float RAD_90 = 90.0f * PI / 180.0f;

/**
 * Camera
 * ------
 */
Vec3f Camera::getDirection() const {
  return getOrientationDirection(orientation);
}

Vec3f Camera::getLeftDirection() const {
  return getOrientationDirection({ 0, orientation.y - RAD_90, 0 });
}

Vec3f Camera::getOrientationDirection(const Vec3f& orientation) const {
  float pitch = orientation.x;
  float yaw = orientation.y;
  float roll = orientation.z;
  float pitchFactor = std::abs(cosf(pitch));

  Vec3f direction = {
    sinf(yaw) * pitchFactor,
    sinf(pitch),
    cosf(yaw) * pitchFactor
  };

  return direction.unit();
}

Vec3f Camera::getRightDirection() const {
  return getOrientationDirection({ 0, orientation.y + RAD_90, 0 });
}