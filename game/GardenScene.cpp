#include <cmath>
#include <algorithm>
#include <subsystem/entities/Model.h>
#include <subsystem/entities/Mesh.h>
#include <subsystem/entities/Skybox.h>
#include <subsystem/entities/Light.h>
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
}

static Vec3f getGroundPosition(float x, float z) {
  return Vec3f(x, getGroundHeight(x, z), z);
}

static Vec3f getRandomGroundPosition() {
  return getGroundPosition(RNG::random(-1200.0f, 1200.0f), RNG::random(-1200.0f, 1200.0f));
}

void GardenScene::addGrass() {
  stage.add<Model>("grass", [&](Model* grass) {
    grass->from(ObjLoader("./game/objects/grass/model.obj"));
    grass->effects = ObjectEffects::GRASS_ANIMATION;
    grass->shadowCascadeLimit = 0;
    grass->isReference = true;
  });

  stage.addMultiple<Model, 10000>([&](Model* blade, int index) {
    blade->from(stage.get<Model>("grass"));
    blade->setScale(RNG::random(5.0f, 15.0f));
    blade->setPosition(getRandomGroundPosition());
    blade->setColor(Vec3f(0.2f, 0.5f, 0.2f));
    blade->rotate(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));

    blade->onUpdate = [=](float dt) {
      Vec3f relativeBladePosition = viewMatrix * blade->position;

      if (relativeBladePosition.z > 0.0f && relativeBladePosition.z < 1250.0f) {
        blade->enable();
      } else {
        blade->disable();
      }
    };
  });
}

void GardenScene::addRocks() {
  stage.add<Model>("rock", [&](Model* rock) {
    rock->from(ObjLoader("./game/objects/rock-1/model.obj"));
    rock->texture = assets.createTexture("./game/objects/rock-1/texture.png");
    rock->normalMap = assets.createTexture("./game/objects/rock-1/normals.png");
    rock->isReference = true;
  });

  stage.addMultiple<Model, 20>([&](Model* rock, int index) {
    rock->from(stage.get<Model>("rock"));
    rock->setPosition(getRandomGroundPosition());
    rock->setScale(RNG::random(15.0f, 30.0f));
    rock->setOrientation(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
  });
}

void GardenScene::addTrees() {
  stage.add<Model>("tree", [&](Model* tree) {
    tree->from(ObjLoader("./game/objects/pine-tree/trunk-model.obj"));
    tree->texture = assets.createTexture("./game/objects/pine-tree/bark-texture.png");
    tree->normalMap = assets.createTexture("./game/objects/pine-tree/bark-normals.png");
    tree->isReference = true;
  });

  stage.add<Model>("leaves", [&](Model* leaves) {
    leaves->from(ObjLoader("./game/objects/pine-tree/leaves1-model.obj"));
    leaves->texture = assets.createTexture("./game/objects/pine-tree/leaves1-texture.png");
    leaves->isReference = true;
  });

  stage.add<Model>("leaves-2", [&](Model* leaves) {
    leaves->from(ObjLoader("./game/objects/pine-tree/leaves2-model.obj"));
    leaves->texture = assets.createTexture("./game/objects/pine-tree/leaves2-texture.png");
    leaves->isReference = true;
  });

  stage.add<Model>("mushroom-base", [&](Model* mushroomBase) {
    mushroomBase->from(ObjLoader("./game/objects/mushroom/base-model.obj"));
    mushroomBase->isReference = true;
  });

  stage.add<Model>("mushroom-head", [&](Model* mushroomHead) {
    mushroomHead->from(ObjLoader("./game/objects/mushroom/head-model.obj"));
    mushroomHead->isEmissive = true;
    mushroomHead->isReference = true;
  });

  stage.addMultiple<Model, 10>([&](Model* tree, int index) {
    Vec3f position = getRandomGroundPosition();
    Vec3f mushroomPosition = position + Vec3f(RNG::random(-30.0f, 30.0f), 0.0f, RNG::random(-30.0f, 30.0f));
    Vec3f orientation(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f);
    float scale = 40.0f;

    mushroomPosition.y = getGroundHeight(mushroomPosition.x, mushroomPosition.z);

    tree->from(stage.get<Model>("tree"));
    tree->setScale(scale);
    tree->setColor(Vec3f(0.2f, 0.175f, 0.05f));
    tree->setOrientation(orientation);
    tree->setPosition(position);

    stage.add<Model>([&](Model* leaves) {
      leaves->from(stage.get<Model>("leaves"));
      leaves->setScale(scale);
      leaves->setPosition(position);
      leaves->setOrientation(orientation);
    });

    stage.add<Model>([&](Model* leaves2) {
      leaves2->from(stage.get<Model>("leaves-2"));
      leaves2->setScale(scale);
      leaves2->setPosition(position);
      leaves2->setOrientation(orientation);
    });

    stage.add<Model>([&](Model* mushroomBase) {
      mushroomBase->from(stage.get<Model>("mushroom-base"));
      mushroomBase->setScale(4.0f);
      mushroomBase->setColor(Vec3f(0.8f, 0.65f, 0.5f));
      mushroomBase->setOrientation(orientation);
      mushroomBase->setPosition(mushroomPosition);
    });

    stage.add<Model>([&](Model* mushroomHead) {
      mushroomHead->from(stage.get<Model>("mushroom-head"));
      mushroomHead->setScale(4.0f);
      mushroomHead->setOrientation(orientation);
      mushroomHead->setPosition(mushroomPosition);
      mushroomHead->setColor(Vec3f(0.2f, 1.0f, 0.2f));

      Vec3f lightColor = Vec3f(0.2f, 1.0f, 0.4f);

      stage.add<Light>([&](Light* light) {
        light->position = mushroomPosition + Vec3f(0.0f, 10.0f, 0.0f);
        light->color = lightColor;
        light->radius = 150.0f;
      });

      stage.add<Light>([&](Light* light) {
        light->position = mushroomPosition + Vec3f(0.0f, 10.0f, 0.0f);
        light->color = lightColor;
        light->radius = 150.0f;
        light->canCastShadows = true;

        light->onUpdate = [=](float dt) {
          float lightDistance = (light->position - camera.position).magnitude();

          if (lightDistance > 150.0f) {
            float attenuation = (lightDistance - 150.0f) / 100.0f;

            light->power = std::max(1.0f - attenuation, 0.0f);
          } else {
            light->power = 1.0f;
          }
        };
      });
    });
  });
}

void GardenScene::onInit() {
  stage.add<Model>("seed", [](Model* seed) {
    seed->from(ObjLoader("./game/objects/seed/model.obj"));
    seed->isReference = true;
  });

  stage.add<Model>("sprout", [](Model* sprout) {
    sprout->from(ObjLoader("./game/objects/sprout/model.obj"));
    sprout->effects = ObjectEffects::GRASS_ANIMATION;
    sprout->shadowCascadeLimit = 2;
    sprout->isReference = true;
  });

  stage.add<Model>("flower-stalk", [](Model* flowerStalk) {
    flowerStalk->from(ObjLoader("./game/objects/small-flower/stalk-model.obj"));
    flowerStalk->effects = ObjectEffects::GRASS_ANIMATION;
    flowerStalk->shadowCascadeLimit = 2;
    flowerStalk->isReference = true;
  });
  
  stage.add<Model>("flower-petals", [&](Model* flowerPetals) {
    flowerPetals->from(ObjLoader("./game/objects/small-flower/petals-model.obj"));
    flowerPetals->normalMap = assets.createTexture("./game/objects/small-flower/petals-normals.png");
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION | ObjectEffects::GRASS_ANIMATION;
    flowerPetals->shadowCascadeLimit = 2;
    flowerPetals->isReference = true;
  });

  stage.add<Model>("lavender-stalk", [&](Model* lavenderStalk) {
    lavenderStalk->from(ObjLoader("./game/objects/lavender/stalk-model.obj"));
    lavenderStalk->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderStalk->shadowCascadeLimit = 2;
    lavenderStalk->isReference = true;
  });

  stage.add<Model>("lavender-flowers", [&](Model* lavenderFlowers) {
    lavenderFlowers->from(ObjLoader("./game/objects/lavender/flowers-model.obj"));
    lavenderFlowers->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderFlowers->shadowCascadeLimit = 2;
    lavenderFlowers->isReference = true;
  });

  stage.add<Model>("lantern", [&](Model* lantern) {
    lantern->from(ObjLoader("./game/objects/lantern/model.obj"));
    lantern->texture = assets.createTexture("./game/objects/lantern/texture.png");
    lantern->normalMap = assets.createTexture("./game/objects/lantern/normals.png");
    lantern->isReference = true;
  });

  stage.addMultiple<Model, 10>([&](Model* lantern, int index) {
    Vec3f lanternPosition = getRandomGroundPosition() - Vec3f(0.0f, 5.0f, 0.0f);
    Vec3f lightPosition = lanternPosition + Vec3f(0.0f, 75.0f, 0.0f);

    lantern->from(stage.get<Model>("lantern"));
    lantern->setScale(100.0f);
    lantern->setPosition(lanternPosition);

    stage.add<Light>([=](Light* light) {
      light->color = Vec3f(1.0f, 0.5f, 0.2f);
      light->position = lightPosition;
      light->radius = 750.0f;
      light->power = 4.0f;

      float offset = RNG::random(0.0f, M_PI);

      light->onUpdate = [=](float dt) {
        light->power = 4.0f + (sinf(getRunningTime() * 10.0f + offset) + sinf(getRunningTime() * 21.3f + offset)) * 0.75f;
      };
    });
  });

  stage.add<Light>([](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.5f, 1.0f);
    light->direction = Vec3f(-1.0f, -0.5f, 0.25f);
    light->power = 0.5f;
    light->canCastShadows = true;
  });

  stage.add<Light>([](auto* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.2f, 0.3f);
    light->direction = Vec3f(0.0f, -1.0f, 0.0f);
  });

  stage.add<Mesh>([&](Mesh* mesh) {
    float tileSize = 40.0f;

    mesh->setSize(60, 60, tileSize, Vec2f(5.0f, 5.0f));
    mesh->setPosition(Vec3f(0.0f));
    mesh->texture = assets.createTexture("./game/objects/ground/grass-texture.png");
    mesh->shadowCascadeLimit = 0;

    mesh->displace([=](Vec3f& vertex, int x, int z) {
      float properX = x * tileSize - 1200.0f;
      float properZ = 1200.0f - z * tileSize;

      vertex.y += getGroundHeight(properX, properZ);
    });
  });

  stage.add<Skybox>([&](Skybox* skybox) {
    skybox->from(assets.createTexture("./game/objects/skybox/night.png"));
    skybox->setScale(5000.0f);

    skybox->onUpdate = [=](float dt) {
      skybox->setPosition(camera.position);
    };
  });

  addGrass();
  addRocks();
  addTrees();

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
  float speedFactor = (inputSystem.isKeyHeld(Key::SHIFT) ? 70.0f : 20.0f);

  viewMatrix = Matrix4::rotate(camera.orientation * Vec3f(1.0f, -1.0f, 1.0f)) * Matrix4::translate(camera.position.invert());

  if (inputSystem.isKeyHeld(Key::W)) {
    velocity += camera.getDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::A)) {
    velocity += camera.getLeftDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::S)) {
    velocity -= camera.getDirection().xz() * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::D)) {
    velocity += camera.getRightDirection().xz() * speedFactor;
  }

  if (velocity.magnitude() > 500.0f) {
    velocity *= (500.0f / velocity.magnitude());
  }

  camera.position += velocity * dt;
  camera.position.y = 60.0f + getGroundHeight(camera.position.x, camera.position.z);

  velocity *= 0.8f;
}

void GardenScene::spawnFlower(float x, float z) {
  Vec3f position = getGroundPosition(x, z);
  Vec3f orientation = Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f);
  float scale = RNG::random(6.0f, 10.0f);

  auto toggleVisibility = [&](Object* object) {
    Vec3f relativePosition = viewMatrix * object->position;

    if (relativePosition.z > 0.0f) {
      object->enable();
    } else {
      object->disable();
    }
  };

  stage.add<Model>([&](Model* flowerStalk) {
    flowerStalk->from(stage.get<Model>("flower-stalk"));
    flowerStalk->setPosition(position);
    flowerStalk->setOrientation(orientation);
    flowerStalk->color = Vec3f(0.3f, 1.0f, 0.4f);

    auto timer = getTimer();

    flowerStalk->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        flowerStalk->setScale(Easing::bounceOut(t) * scale);
      }

      toggleVisibility(flowerStalk);
    };
  });

  stage.add<Model>([&](Model* flowerPetals) {
    flowerPetals->from(stage.get<Model>("flower-petals"));
    flowerPetals->setPosition(position);
    flowerPetals->setOrientation(orientation);
    flowerPetals->color = Vec3f(RNG::random(), RNG::random(), RNG::random());
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION;

    auto timer = getTimer();

    flowerPetals->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        flowerPetals->setScale(Easing::bounceOut(t) * scale);
      }

      toggleVisibility(flowerPetals);
    };
  });
}

void GardenScene::spawnLavender(float x, float z) {
  Vec3f position = getGroundPosition(x, z);
  Vec3f orientation = Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f);
  float scale = RNG::random(15.0f, 20.0f);

  auto toggleVisibility = [&](Object* object) {
    Vec3f relativePosition = viewMatrix * object->position;

    if (relativePosition.z > 0.0f) {
      object->enable();
    } else {
      object->disable();
    }
  };

  stage.add<Model>([&](Model* stalk) {
    stalk->from(stage.get<Model>("lavender-stalk"));
    stalk->setPosition(position);
    stalk->setOrientation(orientation);
    stalk->setColor(Vec3f(0.2f, 0.75f, 0.4f));

    auto timer = getTimer();

    stalk->onUpdate = [=](float dt) {
      float t = timer() / 2.0f;

      if (t <= 1.0f) {
        stalk->setScale(Easing::bounceOut(t) * scale);
      }

      toggleVisibility(stalk);
    };
  });

  stage.add<Model>([&](Model* flowers) {
    flowers->from(stage.get<Model>("lavender-flowers"));
    flowers->setPosition(position);
    flowers->setOrientation(orientation);
    flowers->setColor(Vec3f(0.5f, 0.2f, 0.8f));

    auto timer = getTimer();

    flowers->onUpdate = [=](float dt) {
      float t = timer() / 2.0f;

      if (t <= 1.0f) {
        flowers->setScale(Easing::bounceOut(t) * scale);
      }

      toggleVisibility(flowers);
    };
  });
}

void GardenScene::spawnSprout(float x, float z) {
  auto toggleVisibility = [&](Object* object) {
    Vec3f relativePosition = viewMatrix * object->position;

    if (relativePosition.z > 0.0f) {
      object->enable();
    } else {
      object->disable();
    }
  };

  stage.add<Model>([&](Model* sprout) {
    sprout->from(stage.get<Model>("sprout"));
    sprout->setPosition(getGroundPosition(x, z));
    sprout->setOrientation(Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f));
    sprout->color = Vec3f(0.25f, 1.0f, 0.5f);

    auto timer = getTimer();

    sprout->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        sprout->setScale(Easing::bounceOut(t) * 5.0f);
      }

      toggleVisibility(sprout);
    };
  });
}

void GardenScene::throwSeeds() {
  bool shouldSpawnLavender = RNG::random() < 0.2f;

  stage.addMultiple<Model, 5>([&](Model* seed, int index) {
    seed->from(stage.get<Model>("seed"));
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

      float groundHeight = getGroundHeight(seed->position.x, seed->position.z);

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