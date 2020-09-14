#pragma once

#include <subsystem/Math.h>

#include "subsystem/entities/Actor.h"

class GrassField : public Actor {
public:
  void onInit() override;
  void onRegistered() override;
  void onUpdate(float dt) override;

private:
  Matrix4 viewMatrix = Matrix4::identity();
};