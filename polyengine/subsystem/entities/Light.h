#pragma once

#include "subsystem/entities/Entity.h"
#include "subsystem/traits/Positionable.h"
#include "subsystem/Math.h"

struct Light : Entity, public Positionable {
  enum LightType {
    POINT = 0,
    DIRECTIONAL = 1,
    SPOTLIGHT = 2
  };

  Light() {};
  Light(const Vec3f& position, const Vec3f& color, float radius);

  LightType type = LightType::POINT;
  Vec3f color = Vec3f(1.0f);
  Vec3f direction;
  float radius = 100.0f;
  float power = 1.0f;
  bool canCastShadows = false;

  void setPosition(const Vec3f& position) override;
};