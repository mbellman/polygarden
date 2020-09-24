#pragma once

#include "subsystem/Math.h"

class Positionable {
public:
  virtual void setPosition(const Vec3f& position) = 0;
};