#include "subsystem/entities/Light.h"

/**
 * Light
 * -----
 */
Light::Light(const Vec3f& position, const Vec3f& color, float radius) {
  this->position = position;
  this->color = color;
  this->radius = radius;
}