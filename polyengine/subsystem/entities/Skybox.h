#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/Texture.h"
#include "subsystem/Math.h"

class Skybox : public Object {
public:
  Skybox();

private:
  static Vec3f vertexPositions[14];
  static Vec2f uvs[14];
};