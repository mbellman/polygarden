#pragma once

#include <PolyEngine.h>

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