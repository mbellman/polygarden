#pragma once

#include <subsystem/entities/Actor.h>
#include <subsystem/entities/Light.h>
#include <subsystem/Math.h>

class ProximalShadowLight : public Actor {
public:
  void onInit() override;
  void onUpdate(float dt) override;
  void setColor(const Vec3f& color);
  void setRadius(float radius);

private:
  Light* shadowLight = nullptr;
  Light* fallbackLight = nullptr;
};