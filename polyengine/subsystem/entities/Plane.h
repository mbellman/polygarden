#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/Math.h"

class Plane : public Object {
public:
  void displaceVertices(std::function<void(Vec3f&, int, int)> offsetHandler);
  void setSize(int w, int h, float tileSize, Vec2f textureCycle = Vec2f(1.0f, 1.0f));

private:
  int width;
  int height;
};