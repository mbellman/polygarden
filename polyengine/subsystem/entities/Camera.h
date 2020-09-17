#pragma once

#include "subsystem/entities/Entity.h"
#include "subsystem/Math.h"

class Camera : public Entity {
public:
  static const Camera* active;

  float fov = 90.0f;

  // TODO: Create 'Orientation' struct with below methods
  Vec3f getDirection() const;
  Vec3f getLeftDirection() const;
  Vec3f getOrientationDirection(const Vec3f& orientation) const;
  Vec3f getRightDirection() const;
};