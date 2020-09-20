#include <cmath>

#include <subsystem/RNG.h>

#include "HeightMap.h"

constexpr static float PI = 3.141592;

float HeightMap::getGroundHeight(float x, float z) {
  float fx = PI / 150.0f * x;
  float fz = PI / 150.0f * z;

  return (
    5.0f * (sinf(fx) + cosf(fz)) +
    25.0f * sinf(fx * 0.3f) +
    30.0f * sinf(fz * 0.2f)
  );
}

Vec3f HeightMap::getGroundPosition(float x, float z) {
  return Vec3f(x, getGroundHeight(x, z), z);
}

Vec3f HeightMap::getRandomGroundPosition() {
  return getGroundPosition(RNG::random(-1150.0f, 1150.0f), RNG::random(-1150.0f, 1150.0f));
}