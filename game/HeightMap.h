#pragma once

#include <subsystem/Math.h>

namespace HeightMap {
  float getGroundHeight(float x, float z);
  Vec3f getGroundPosition(float x, float z);
  Vec3f getRandomGroundPosition();
}