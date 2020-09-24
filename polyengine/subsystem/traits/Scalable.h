#pragma once

#include "subsystem/Math.h"

class Scalable {
public:
  virtual void setScale(const Vec3f& scale) = 0;
  virtual void setScale(float scale) final;
};