#include <cmath>
#include <subsystem/Entities.h>
#include <subsystem/ObjLoader.h>
#include <subsystem/RNG.h>

#include "GardenScene.h"
#include "Easing.h"

static float getMapHeight(float x, float z) {
  float fx = M_PI / 150.0f * x;
  float fz = M_PI / 150.0f * z;

  return (
    5.0f * (sinf(fx) + cosf(fz)) +
    20.0f * sinf(fx * 0.3f) +
    10.0f * cosf(fz * 0.2f)
  );
};

void GardenScene::addFlower() {
  Vec3f position = camera.position + camera.getDirection().xz() * 100.0f;

  position.y = getMapHeight(1250.0f + position.x , -position.z + 1250.0f);

  stage.add<Model>([&](Model* sprout) {
    sprout->from(modelMap.at("sprout"));
    sprout->setPosition(position);
    sprout->setScale(10.0f);
    sprout->setOrientation(Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f));

    float spawnTime = getRunningTime();

    sprout->onUpdate = [=](float dt) {
      float t = (getRunningTime() - spawnTime) / 1.0f;

      sprout->setScale(Easing::bounceOut(t) * 5.0f);
    };
  });
}

void GardenScene::onInit() {
  ObjLoader sproutObj("./game/sprout.obj");
  ObjLoader lanternObj("./game/lantern.obj");
  Model* sprout = new Model();

  sprout->from(sproutObj);
  sprout->setColor(Vec3f(0.25f, 1.0f, 0.5f));

  modelMap.emplace("sprout", sprout);

  for (int i = 0; i < 10; i++) {
    stage.add<Model>([=](Model* lantern) {
      float x = RNG::random() * 2500.0f - 1250.0f;
      float z = RNG::random() * 2500.0f - 1250.0f;
      float y = getMapHeight(1250.0f + x, -z + 1250.0f) - 5.0f;

      lantern->from(lanternObj);
      lantern->texture = assets.createTexture("./game/lantern-texture.png");
      lantern->normalMap = assets.createTexture("./game/lantern-normal-map.png");
      lantern->setScale(100.0f);
      lantern->setPosition(Vec3f(x, y, z));

      stage.add<Light>([=](Light* light) {
        light->color = Vec3f(1.0f, 1.0f, 0.2f);
        light->position = Vec3f(x, y + 75.0f, z);
        light->radius = 750.0f;
        light->power = 4.0f;
      });
    });
  }

  stage.add<Light>([=](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(1.0f, 1.0f, 0.5f);
    light->direction = Vec3f(1.0f, -0.5f, 1.0f);
    light->canCastShadows = true;
  });

  stage.add<Light>([](auto* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.2f, 0.3f);
    light->direction = Vec3f(0.0f, -1.0f, 0.0f);
  });

  stage.add<Mesh>([=](Mesh* mesh) {
    mesh->setSize(250, 250, 10.0f);
    mesh->setColor(Vec3f(0.5f));
    mesh->setPosition(Vec3f(0.0f));

    mesh->texture = assets.createTexture("./game/grass-texture.png");

    mesh->displace([=](Vec3f& vertex, int x, int z) {
      vertex.y += getMapHeight(x * 10.0f, z * 10.0f);
    });
  });

  stage.add<Skybox>([=](Skybox* skybox) {
    skybox->from(assets.createTexture("./game/dummy-day-skybox.png"));
    skybox->setScale(5000.0f);
    skybox->setColor(Vec3f(1.0f));
  });

  inputSystem.onMouseMotion([=](const SDL_MouseMotionEvent& event) {
    if (SDL_GetRelativeMouseMode()) {
      camera.orientation.x -= event.yrel / 1000.0f;
      camera.orientation.y += event.xrel / 1000.0f;
    }
  });

  inputSystem.onMouseButton([=](const SDL_MouseButtonEvent& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
        addFlower();
      }

      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
  });

  inputSystem.onKeyDown([=](const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_ESCAPE) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
    }
  });
}

void GardenScene::onUpdate(float dt) {
  float speedFactor = dt * (inputSystem.isKeyHeld(Key::SHIFT) ? 200.0f : 100.0f);

  if (inputSystem.isKeyHeld(Key::W)) {
    camera.position += camera.getDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::A)) {
    camera.position += camera.getLeftDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::S)) {
    camera.position -= camera.getDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::D)) {
    camera.position += camera.getRightDirection().xz() * speedFactor;
  }

  camera.position.y = 60.0f + getMapHeight(1250.0f + camera.position.x, -camera.position.z + 1250.0f);
}