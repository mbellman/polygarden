#include <cmath>
#include <algorithm>

#include <PolyEngine.h>

#include "GardenScene.h"
#include "HeightMap.h"
#include "Easing.h"
#include "actors/GrassField.h"
#include "actors/Background.h"
#include "actors/Boundary.h"
#include "actors/Rock.h"
#include "actors/ProximalShadowLight.h"
#include "actors/VisibleObjectFilter.h"

void GardenScene::addTrees() {
  stage.add<ReferenceMesh>("tree", [&](ReferenceMesh* tree) {
    auto* shadowLod = new Mesh();

    shadowLod->from(ObjLoader("./assets/pine-tree/trunk-model-lod.obj"));

    tree->from(ObjLoader("./assets/pine-tree/trunk-model.obj"));
    tree->texture = Texture::use("./assets/pine-tree/bark-texture.png");
    tree->normalMap = Texture::use("./assets/pine-tree/bark-normals.png");
    tree->shadowLod = shadowLod;
  });

  stage.add<ReferenceMesh>("leaves", [&](ReferenceMesh* leaves) {
    leaves->from(ObjLoader("./assets/pine-tree/leaves1-model.obj"));
    leaves->texture = Texture::use("./assets/pine-tree/leaves1-texture.png");
  });

  stage.add<ReferenceMesh>("leaves-2", [&](ReferenceMesh* leaves) {
    leaves->from(ObjLoader("./assets/pine-tree/leaves2-model.obj"));
    leaves->texture = Texture::use("./assets/pine-tree/leaves2-texture.png");
  });

  stage.add<ReferenceMesh>("mushroom-base", [&](ReferenceMesh* mushroomBase) {
    mushroomBase->from(ObjLoader("./assets/mushroom/base-model.obj"));
  });

  stage.add<ReferenceMesh>("mushroom-head", [&](ReferenceMesh* mushroomHead) {
    mushroomHead->from(ObjLoader("./assets/mushroom/head-model.obj"));
    mushroomHead->isEmissive = true;
  });

  stage.addMultiple<Instance, 10>([&](Instance* tree, int index) {
    Vec3f position = HeightMap::getRandomGroundPosition();
    float scale = 40.0f;
    Vec3f mushroomPosition = position + Vec3f(RNG::random(-scale, scale), 0.0f, RNG::random(-scale, scale));
    Vec3f orientation(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f);

    mushroomPosition.y = HeightMap::getGroundHeight(mushroomPosition.x, mushroomPosition.z);

    tree->from(stage.get<Mesh>("tree"));
    tree->setScale(scale);
    tree->setColor(Vec3f(0.2f, 0.175f, 0.05f));
    tree->setOrientation(orientation);
    tree->setPosition(position);

    stage.add<Instance>([&](Instance* leaves) {
      leaves->from(stage.get<Mesh>("leaves"));
      leaves->setScale(scale);
      leaves->setPosition(position);
      leaves->setOrientation(orientation);
    });

    stage.add<Instance>([&](Instance* leaves2) {
      leaves2->from(stage.get<Mesh>("leaves-2"));
      leaves2->setScale(scale);
      leaves2->setPosition(position);
      leaves2->setOrientation(orientation);
    });

    stage.add<Instance>([&](Instance* mushroomBase) {
      mushroomBase->from(stage.get<Mesh>("mushroom-base"));
      mushroomBase->setScale(4.0f);
      mushroomBase->setColor(Vec3f(0.8f, 0.65f, 0.5f));
      mushroomBase->setOrientation(orientation);
      mushroomBase->setPosition(mushroomPosition);
    });

    stage.add<Instance>([&](Instance* mushroomHead) {
      mushroomHead->from(stage.get<Mesh>("mushroom-head"));
      mushroomHead->setScale(4.0f);
      mushroomHead->setOrientation(orientation);
      mushroomHead->setPosition(mushroomPosition);
      mushroomHead->setColor(Vec3f(0.2f, 1.0f, 0.2f));
    });

    stage.add<ProximalShadowLight>([&](ProximalShadowLight* light) {
      light->setPosition(mushroomPosition + Vec3f(0.0f, 10.0f, 0.0f));
      light->setColor(Vec3f(0.2f, 1.0f, 0.4f));
      light->setRadius(250.0f);
      light->setShadowMapSize({ 512, 512 });
    });
  });
}

void GardenScene::onInit() {
  stage.add<VisibleObjectFilter>("object-filter");

  stage.add<ReferenceMesh>("seed", [](ReferenceMesh* seed) {
    seed->from(ObjLoader("./assets/seed/model.obj"));
  });

  stage.add<ReferenceMesh>("sprout", [&](ReferenceMesh* sprout) {
    sprout->from(ObjLoader("./assets/sprout/model.obj"));
    sprout->normalMap = Texture::use("./assets/sprout/normals.png");
    sprout->effects = ObjectEffects::GRASS_ANIMATION;
    sprout->shadowCascadeLimit = 2;
  });

  stage.add<ReferenceMesh>("flower-stalk", [](ReferenceMesh* flowerStalk) {
    flowerStalk->from(ObjLoader("./assets/small-flower/stalk-model.obj"));
    flowerStalk->effects = ObjectEffects::GRASS_ANIMATION;
    flowerStalk->shadowCascadeLimit = 2;
  });
  
  stage.add<ReferenceMesh>("flower-petals", [&](ReferenceMesh* flowerPetals) {
    flowerPetals->from(ObjLoader("./assets/small-flower/petals-model.obj"));
    flowerPetals->normalMap = Texture::use("./assets/small-flower/petals-normals.png");
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION | ObjectEffects::GRASS_ANIMATION;
    flowerPetals->shadowCascadeLimit = 2;
  });

  stage.add<ReferenceMesh>("lavender-stalk", [&](ReferenceMesh* lavenderStalk) {
    lavenderStalk->from(ObjLoader("./assets/lavender/stalk-model.obj"));
    lavenderStalk->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderStalk->shadowCascadeLimit = 2;
  });

  stage.add<ReferenceMesh>("lavender-flowers", [&](ReferenceMesh* lavenderFlowers) {
    lavenderFlowers->from(ObjLoader("./assets/lavender/flowers-model.obj"));
    lavenderFlowers->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderFlowers->shadowCascadeLimit = 2;
  });

  stage.add<ReferenceMesh>("lantern", [&](ReferenceMesh* lantern) {
    lantern->from(ObjLoader("./assets/lantern/model.obj"));
    lantern->texture = Texture::use("./assets/lantern/texture.png");
    lantern->normalMap = Texture::use("./assets/lantern/normals.png");
  });

  stage.addMultiple<Instance, 10>([&](Instance* lantern, int index) {
    Vec3f lanternPosition = HeightMap::getRandomGroundPosition() - Vec3f(0.0f, 3.0f, 0.0f);
    Vec3f lightPosition = lanternPosition + Vec3f(0.0f, 35.5f, 0.0f);

    lantern->from(stage.get<Mesh>("lantern"));
    lantern->setScale(50.0f);
    lantern->setPosition(lanternPosition);

    stage.add<Light>([=](Light* light) {
      light->color = Vec3f(1.0f, 0.5f, 0.2f);
      light->position = lightPosition;
      light->radius = 250.0f;
      light->power = 5.0f;

      float offset = RNG::random(0.0f, M_PI);

      light->onUpdate = [=](float dt) {
        light->power = 5.0f + (sinf(getRunningTime() * 5.0f + offset) + sinf(getRunningTime() * 21.3f + offset)) * 0.75f;
      };
    });
  });

  stage.add<GrassField>();
  stage.add<Background>();
  stage.add<Boundary>();
  stage.addMultiple<Rock, 20>();

  addTrees();

  stage.get<VisibleObjectFilter>("object-filter")->addObjects({
    stage.get("sprout"),
    stage.get("flower-stalk"),
    stage.get("flower-petals"),
    stage.get("lavender-stalk"),
    stage.get("lavender-flowers"),
    stage.get("grass")
  });

  input.onMouseMotion([=](const SDL_MouseMotionEvent& event) {
    if (SDL_GetRelativeMouseMode()) {
      camera.orientation.x += event.yrel / 1000.0f;
      camera.orientation.y += event.xrel / 1000.0f;
    }
  });

  input.onMouseButton([=](const SDL_MouseButtonEvent& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
        throwSeeds();
      }

      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
  });

  input.onKeyDown([=](const SDL_KeyboardEvent& event) {
    if (event.keysym.sym == SDLK_ESCAPE) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
    }
  });
}

void GardenScene::onUpdate(float dt) {
  super::onUpdate(dt);

  float speedFactor = (input.isKeyHeld(Key::SHIFT) ? 70.0f : 20.0f);

  if (input.isKeyHeld(Key::W)) {
    velocity += camera.getDirection().xz() * speedFactor;
  }

  if (input.isKeyHeld(Key::A)) {
    velocity += camera.getLeftDirection().xz() * speedFactor;
  }

  if (input.isKeyHeld(Key::S)) {
    velocity -= camera.getDirection().xz() * speedFactor;
  }

  if (input.isKeyHeld(Key::D)) {
    velocity += camera.getRightDirection().xz() * speedFactor;
  }

  if (velocity.magnitude() > 500.0f) {
    velocity *= (500.0f / velocity.magnitude());
  }

  camera.position += velocity * dt;
  camera.position.y = 60.0f + HeightMap::getGroundHeight(camera.position.x, camera.position.z);

  velocity *= 0.8f;
}

void GardenScene::spawnFlower(float x, float z) {
  Vec3f position = HeightMap::getGroundPosition(x, z) - Vec3f(0.0f, 2.0f, 0.0f);
  Vec3f orientation = Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f);
  float scale = RNG::random(6.0f, 10.0f);

  stage.add<Instance>([&](Instance* flowerStalk) {
    flowerStalk->from(stage.get<Mesh>("flower-stalk"));
    flowerStalk->setPosition(position);
    flowerStalk->setOrientation(orientation);
    flowerStalk->color = Vec3f(0.3f, 1.0f, 0.4f);

    auto timer = createTimer();

    flowerStalk->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        flowerStalk->setScale(Easing::bounceOut(t) * scale);
      }
    };
  });

  stage.add<Instance>([&](Instance* flowerPetals) {
    flowerPetals->from(stage.get<Mesh>("flower-petals"));
    flowerPetals->setPosition(position);
    flowerPetals->setOrientation(orientation);
    flowerPetals->color = Vec3f(RNG::random(), RNG::random(), RNG::random());
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION;

    auto timer = createTimer();

    flowerPetals->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        flowerPetals->setScale(Easing::bounceOut(t) * scale);
      }
    };
  });
}

void GardenScene::spawnLavender(float x, float z) {
  Vec3f position = HeightMap::getGroundPosition(x, z);
  Vec3f orientation = Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f);
  float scale = RNG::random(15.0f, 20.0f);

  stage.add<Instance>([&](Instance* stalk) {
    stalk->from(stage.get<Mesh>("lavender-stalk"));
    stalk->setPosition(position);
    stalk->setOrientation(orientation);
    stalk->setColor(Vec3f(0.2f, 0.75f, 0.4f));

    auto timer = createTimer();

    stalk->onUpdate = [=](float dt) {
      float t = timer() / 2.0f;

      if (t <= 1.0f) {
        stalk->setScale(Easing::bounceOut(t) * scale);
      }
    };
  });

  stage.add<Instance>([&](Instance* flowers) {
    flowers->from(stage.get<Mesh>("lavender-flowers"));
    flowers->setPosition(position);
    flowers->setOrientation(orientation);
    flowers->setColor(Vec3f(0.5f, 0.2f, 0.8f));

    auto timer = createTimer();

    flowers->onUpdate = [=](float dt) {
      float t = timer() / 2.0f;

      if (t <= 1.0f) {
        flowers->setScale(Easing::bounceOut(t) * scale);
      }
    };
  });
}

void GardenScene::spawnSprout(float x, float z) {
  stage.add<Instance>([&](Instance* sprout) {
    sprout->from(stage.get<Mesh>("sprout"));
    sprout->setPosition(HeightMap::getGroundPosition(x, z) - Vec3f(0.0f, 1.0f, 0.0f));
    sprout->setOrientation(Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f));
    sprout->color = Vec3f(0.1f, RNG::random(0.75f, 1.0f), RNG::random(0.1f, 0.3f));

    auto timer = createTimer();

    sprout->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        sprout->setScale(Easing::bounceOut(t) * 5.0f);
      }
    };
  });
}

void GardenScene::throwSeeds() {
  bool shouldSpawnLavender = RNG::random() < 0.2f;

  stage.addMultiple<Instance, 5>([&](Instance* seed, int index) {
    seed->from(stage.get<Mesh>("seed"));
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
      velocity.y -= 100.0f * dt;

      seed->move(velocity * dt);

      float groundHeight = HeightMap::getGroundHeight(seed->position.x, seed->position.z);

      if (seed->position.y < groundHeight) {
        if (!shouldSpawnLavender) {
          if (RNG::random() < 0.2f) {
            spawnFlower(seed->position.x, seed->position.z);
          } else {
            spawnSprout(seed->position.x, seed->position.z);
          }
        } else {
          spawnLavender(seed->position.x, seed->position.z);
        }

        seed->expire();
      }
    };
  });
}