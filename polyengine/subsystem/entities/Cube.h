#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/Math.h"

class Cube : public Object {
public:
  Cube();

private:
  static Vec3f corners[8];
  static Vec2f uvs[4];
  static int faces[6][4];
};