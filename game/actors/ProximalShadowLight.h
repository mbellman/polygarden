#pragma once

#include <subsystem/entities/Actor.h>
#include <subsystem/entities/Light.h>
#include <subsystem/Math.h>

class ProximalShadowLight : public Actor {
public:
  void onInit() override;
  void onAdded() override;
  void onUpdate(float dt) override;
  void setColor(const Vec3f& color);
  void setRadius(float radius);
  void setShadowMapSize(const Area<unsigned int>& shadowMapSize);

private:
  Light* shadowLight = nullptr;
  Light* fallbackLight = nullptr;
};