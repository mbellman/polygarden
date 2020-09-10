#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/Texture.h"
#include "subsystem/Math.h"

class Skybox : public Object {
public:
  void from(const Texture* texture);

private:
  static Vec3f vertexPositions[14];
  static Vec2f uvs[14];
};