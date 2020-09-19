#include <algorithm>

#include <subsystem/Stage.h>
#include <subsystem/entities/Camera.h>

#include "actors/ProximalShadowLight.h"

void ProximalShadowLight::onInit() {
  stage->add<Light>([=](Light* light) {
    fallbackLight = light;
  });

  stage->add<Light>([=](Light* light) {
    light->canCastShadows = true;

    shadowLight = light;
  });
}

void ProximalShadowLight::onUpdate(float dt) {
  float lightDistance = fallbackLight->getLocalDistance();
  float attenuationLimit = shadowLight->radius * 0.6f;

  if (lightDistance > attenuationLimit) {
    float attenuation = (lightDistance - attenuationLimit) / 100.0f;

    shadowLight->power = std::max(1.0f - attenuation, 0.0f);
    fallbackLight->power = std::min(attenuation, 1.0f);
  } else {
    shadowLight->power = 1.0f;
    fallbackLight->power = 0.0f;
  }
}

void ProximalShadowLight::setColor(const Vec3f& color) {
  fallbackLight->color = color;
  shadowLight->color = color;
}

void ProximalShadowLight::setPosition(const Vec3f& position) {
  fallbackLight->position = position;
  shadowLight->position = position;
}

void ProximalShadowLight::setRadius(float radius) {
  fallbackLight->radius = radius;
  shadowLight->radius = radius;
}