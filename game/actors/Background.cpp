#include <PolyEngine.h>

#include "actors/Background.h"

void Background::onInit() {
  stage->add<Light>([](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.5f, 1.0f);
    light->direction = Vec3f(-1.0f, -0.6f, 0.25f);
    light->power = 0.5f;
    light->canCastShadows = true;
    light->shadowMapSize = { 2048, 2048 };
  });

  stage->add<Skybox>([&](Skybox* skybox) {
    skybox->setScale(5000.0f);
    skybox->texture = Texture::use("./assets/skybox/night.png");

    skybox->onUpdate = [=](float dt) {
      skybox->setPosition(Camera::active->position);
    };
  });
}