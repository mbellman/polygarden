#include <subsystem/entities/Light.h>
#include <subsystem/entities/Skybox.h>
#include <subsystem/entities/Camera.h>
#include <subsystem/Stage.h>

#include "actors/Background.h"

void Background::onInit() {
  stage->add<Light>([](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.5f, 1.0f);
    light->direction = Vec3f(-1.0f, -0.5f, 0.25f);
    light->power = 0.5f;
    light->canCastShadows = true;
  });

  stage->add<Light>([](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.2f, 0.3f);
    light->direction = Vec3f(0.0f, -1.0f, 0.0f);
    light->power = 0.5f;
  });

  stage->add<Skybox>([&](Skybox* skybox) {
    skybox->setScale(5000.0f);
    skybox->texture = Texture::use("./assets/skybox/night.png");

    skybox->onUpdate = [=](float dt) {
      skybox->setPosition(Camera::active->position);
    };
  });
}