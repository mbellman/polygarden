#pragma once

#include "subsystem/Math.h"

class Orientable {
public:
  virtual void setOrientation(const Vec3f& orientation) = 0;
};