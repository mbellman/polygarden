#include <algorithm>
#include <cstdio>

#include <subsystem/Stage.h>
#include <subsystem/entities/Camera.h>
#include <subsystem/entities/Light.h>

#include "actors/ProximalShadowLight.h"

void ProximalShadowLight::onInit() {
  fallbackLight = new Light();
  shadowLight = new Light();

  shadowLight->canCastShadows = true;

  addPositionable(fallbackLight);
  addPositionable(shadowLight);
}

void ProximalShadowLight::onAdded() {
  stage->add(fallbackLight);
  stage->add(shadowLight);
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

void ProximalShadowLight::setRadius(float radius) {
  fallbackLight->radius = radius;
  shadowLight->radius = radius;
}

void ProximalShadowLight::setShadowMapSize(const Area<unsigned int>& shadowMapSize) {
  fallbackLight->shadowMapSize = shadowMapSize;
  shadowLight->shadowMapSize = shadowMapSize;
}
