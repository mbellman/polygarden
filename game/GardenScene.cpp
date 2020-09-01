#include <cmath>
#include <subsystem/Entities.h>
#include <subsystem/ObjLoader.h>
#include <subsystem/RNG.h>

#include "GardenScene.h"
#include "Easing.h"

static float getGroundHeight(float x, float z) {
  float fx = M_PI / 150.0f * x;
  float fz = M_PI / 150.0f * z;

  return (
    5.0f * (sinf(fx) + cosf(fz)) +
    20.0f * sinf(fx * 0.3f) +
    10.0f * cosf(fz * 0.2f)
  );
};

GardenScene::~GardenScene() {
  for (auto& [key, model] : modelMap) {
    delete model;
  }

  modelMap.clear();
}

void GardenScene::onInit() {
  ObjLoader sproutObj("./game/sprout.obj");
  ObjLoader lanternObj("./game/lantern.obj");
  ObjLoader flowerStalkObj("./game/flower-stalk.obj");
  ObjLoader flowerPetalsObj("./game/flower-petals.obj");

  Model* sprout = new Model();
  Model* flowerStalk = new Model();
  Model* flowerPetals = new Model();
  Model* lantern = new Model();

  sprout->from(sproutObj);
  sprout->isReference = true;

  flowerStalk->from(flowerStalkObj);
  flowerStalk->isReference = true;

  flowerPetals->from(flowerPetalsObj);
  flowerPetals->isReference = true;

  lantern->from(lanternObj);
  lantern->texture = assets.createTexture("./game/lantern-texture.png");
  lantern->normalMap = assets.createTexture("./game/lantern-normal-map.png");
  lantern->isReference = true;

  stage.add(sprout);
  stage.add(flowerStalk);
  stage.add(flowerPetals);
  stage.add(lantern);

  modelMap.emplace("sprout", sprout);
  modelMap.emplace("flower-stalk", flowerStalk);
  modelMap.emplace("flower-petals", flowerPetals);
  modelMap.emplace("lantern", lantern);

  stage.addMultiple<Model, 10>([=](Model* lantern, int index) {
    float x = RNG::random() * 2500.0f - 1250.0f;
    float z = RNG::random() * 2500.0f - 1250.0f;
    float y = getGroundHeight(1250.0f + x, -z + 1250.0f) - 5.0f;

    lantern->from(modelMap.at("lantern"));
    lantern->setScale(100.0f);
    lantern->setPosition(Vec3f(x, y, z));

    stage.add<Light>([=](Light* light) {
      light->color = Vec3f(1.0f, 1.0f, 0.2f);
      light->position = Vec3f(x, y + 75.0f, z);
      light->radius = 750.0f;
      light->power = 4.0f;
    });
  });

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
    mesh->setPosition(Vec3f(0.0f));

    mesh->texture = assets.createTexture("./game/grass-texture.png");

    mesh->displace([=](Vec3f& vertex, int x, int z) {
      vertex.y += getGroundHeight(x * 10.0f, z * 10.0f);
    });
  });

  stage.add<Skybox>([=](Skybox* skybox) {
    skybox->from(assets.createTexture("./game/dummy-day-skybox.png"));
    skybox->setScale(5000.0f);
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
        throwSeeds();
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

  camera.position.y = 60.0f + getGroundHeight(1250.0f + camera.position.x, -camera.position.z + 1250.0f);
}

void GardenScene::spawnFlower(float x, float z) {
  Vec3f position = {
    x,
    getGroundHeight(1250.0f + x, -z + 1250.0f),
    z
  };

  Vec3f orientation = Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f);

  stage.add<Model>([&](Model* flowerStalk) {
    flowerStalk->from(modelMap.at("flower-stalk"));
    flowerStalk->setPosition(position);
    flowerStalk->setOrientation(orientation);
    flowerStalk->color = Vec3f(0.3f, 1.0f, 0.4f);

    float spawnTime = getRunningTime();

    flowerStalk->onUpdate = [=](float dt) {
      float t = (getRunningTime() - spawnTime) / 1.0f;

      if (t <= 1.0f) {
        flowerStalk->setScale(Easing::bounceOut(t) * 8.0f);
      }
    };
  });

  stage.add<Model>([&](Model* flowerPetals) {
    flowerPetals->from(modelMap.at("flower-petals"));
    flowerPetals->setPosition(position);
    flowerPetals->setOrientation(orientation);
    flowerPetals->color = Vec3f(RNG::random(), RNG::random(), RNG::random());

    float spawnTime = getRunningTime();

    flowerPetals->onUpdate = [=](float dt) {
      float t = (getRunningTime() - spawnTime) / 1.0f;

      if (t <= 1.0f) {
        flowerPetals->setScale(Easing::bounceOut(t) * 8.0f);
      }
    };
  });
}

void GardenScene::spawnSprout(float x, float z) {
  stage.add<Model>([&](Model* sprout) {
    Vec3f position = {
      x,
      getGroundHeight(1250.0f + x, -z + 1250.0f),
      z
    };

    sprout->from(modelMap.at("sprout"));
    sprout->setPosition(position);
    sprout->setOrientation(Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f));
    sprout->color = Vec3f(0.25f, 1.0f, 0.5f);

    float spawnTime = getRunningTime();

    sprout->onUpdate = [=](float dt) {
      float t = (getRunningTime() - spawnTime) / 1.0f;

      if (t <= 1.0f) {
        sprout->setScale(Easing::bounceOut(t) * 5.0f);
      }
    };
  });
}

void GardenScene::throwSeeds() {
  ObjLoader seedObj("./game/seed.obj");

  stage.addMultiple<Model, 5>([&](Model* seed, int index) {
    seed->from(seedObj);
    seed->setScale(0.5f);
    seed->setPosition(camera.position + camera.getDirection() * 50.0f);
    seed->color = Vec3f(0.6f, 0.5f, 0.2f);
    seed->lifetime = 2.0f;

    Vec3f velocity = (
      camera.getDirection().xz() +
      camera.getLeftDirection().xz() * RNG::random() +
      camera.getRightDirection().xz() * RNG::random()
    ).unit() * RNG::random(30.0f, 60.0f);

    seed->onUpdate = [=](float dt) mutable {
      velocity.y -= 2.0f;

      seed->move(velocity * dt);

      float groundHeight = getGroundHeight(1250.0f + seed->position.x, -seed->position.z + 1250.0f);

      if (seed->position.y < groundHeight) {
        if (RNG::random() < 0.2f) {
          spawnFlower(seed->position.x, seed->position.z);
        } else {
          spawnSprout(seed->position.x, seed->position.z);
        }

        seed->expire();
      }
    };
  });
}