#include <cmath>

#include "Easing.h"

#define PI 3.141592

float Easing::bounceOut(float t) {
  float a = (2.0f * PI) / 3.0f;

  return (
    t == 0.0f ? 0.0f :
    t >= 1.0f ? 1.0f :
    pow(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * a) + 1.0f
  );
}