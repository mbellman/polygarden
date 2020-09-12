#include <cmath>
#include <algorithm>
#include <subsystem/entities/Mesh.h>
#include <subsystem/entities/Plane.h>
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
  stage.add<Mesh>("grass", [&](Mesh* grass) {
    grass->from(ObjLoader("./assets/grass/model.obj"));
    grass->effects = ObjectEffects::GRASS_ANIMATION;
    grass->shadowCascadeLimit = 0;
    grass->isReference = true;
  });

  stage.addMultiple<Mesh, 10000>([&](Mesh* blade, int index) {
    blade->from(stage.get<Mesh>("grass"));
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
  stage.add<Mesh>("rock", [&](Mesh* rock) {
    rock->from(ObjLoader("./assets/rock-1/model.obj"));
    rock->texture = assets.createTexture("./assets/rock-1/texture.png");
    rock->normalMap = assets.createTexture("./assets/rock-1/normals.png");
    rock->isReference = true;
  });

  stage.addMultiple<Mesh, 20>([&](Mesh* rock, int index) {
    rock->from(stage.get<Mesh>("rock"));
    rock->setPosition(getRandomGroundPosition());
    rock->setScale(RNG::random(15.0f, 30.0f));
    rock->setOrientation(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
  });
}

void GardenScene::addTrees() {
  stage.add<Mesh>("tree", [&](Mesh* tree) {
    tree->from(ObjLoader("./assets/pine-tree/trunk-model.obj"));
    tree->texture = assets.createTexture("./assets/pine-tree/bark-texture.png");
    tree->normalMap = assets.createTexture("./assets/pine-tree/bark-normals.png");
    tree->isReference = true;
  });

  stage.add<Mesh>("leaves", [&](Mesh* leaves) {
    leaves->from(ObjLoader("./assets/pine-tree/leaves1-model.obj"));
    leaves->texture = assets.createTexture("./assets/pine-tree/leaves1-texture.png");
    leaves->isReference = true;
  });

  stage.add<Mesh>("leaves-2", [&](Mesh* leaves) {
    leaves->from(ObjLoader("./assets/pine-tree/leaves2-model.obj"));
    leaves->texture = assets.createTexture("./assets/pine-tree/leaves2-texture.png");
    leaves->isReference = true;
  });

  stage.add<Mesh>("mushroom-base", [&](Mesh* mushroomBase) {
    mushroomBase->from(ObjLoader("./assets/mushroom/base-model.obj"));
    mushroomBase->isReference = true;
  });

  stage.add<Mesh>("mushroom-head", [&](Mesh* mushroomHead) {
    mushroomHead->from(ObjLoader("./assets/mushroom/head-model.obj"));
    mushroomHead->isEmissive = true;
    mushroomHead->isReference = true;
  });

  stage.addMultiple<Mesh, 10>([&](Mesh* tree, int index) {
    Vec3f position = getRandomGroundPosition();
    Vec3f mushroomPosition = position + Vec3f(RNG::random(-30.0f, 30.0f), 0.0f, RNG::random(-30.0f, 30.0f));
    Vec3f orientation(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f);
    float scale = 40.0f;

    mushroomPosition.y = getGroundHeight(mushroomPosition.x, mushroomPosition.z);

    tree->from(stage.get<Mesh>("tree"));
    tree->setScale(scale);
    tree->setColor(Vec3f(0.2f, 0.175f, 0.05f));
    tree->setOrientation(orientation);
    tree->setPosition(position);

    stage.add<Mesh>([&](Mesh* leaves) {
      leaves->from(stage.get<Mesh>("leaves"));
      leaves->setScale(scale);
      leaves->setPosition(position);
      leaves->setOrientation(orientation);
    });

    stage.add<Mesh>([&](Mesh* leaves2) {
      leaves2->from(stage.get<Mesh>("leaves-2"));
      leaves2->setScale(scale);
      leaves2->setPosition(position);
      leaves2->setOrientation(orientation);
    });

    stage.add<Mesh>([&](Mesh* mushroomBase) {
      mushroomBase->from(stage.get<Mesh>("mushroom-base"));
      mushroomBase->setScale(4.0f);
      mushroomBase->setColor(Vec3f(0.8f, 0.65f, 0.5f));
      mushroomBase->setOrientation(orientation);
      mushroomBase->setPosition(mushroomPosition);
    });

    stage.add<Mesh>([&](Mesh* mushroomHead) {
      mushroomHead->from(stage.get<Mesh>("mushroom-head"));
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
  stage.add<Mesh>("seed", [](Mesh* seed) {
    seed->from(ObjLoader("./assets/seed/model.obj"));
    seed->isReference = true;
  });

  stage.add<Mesh>("sprout", [](Mesh* sprout) {
    sprout->from(ObjLoader("./assets/sprout/model.obj"));
    sprout->effects = ObjectEffects::GRASS_ANIMATION;
    sprout->shadowCascadeLimit = 2;
    sprout->isReference = true;
  });

  stage.add<Mesh>("flower-stalk", [](Mesh* flowerStalk) {
    flowerStalk->from(ObjLoader("./assets/small-flower/stalk-model.obj"));
    flowerStalk->effects = ObjectEffects::GRASS_ANIMATION;
    flowerStalk->shadowCascadeLimit = 2;
    flowerStalk->isReference = true;
  });
  
  stage.add<Mesh>("flower-petals", [&](Mesh* flowerPetals) {
    flowerPetals->from(ObjLoader("./assets/small-flower/petals-model.obj"));
    flowerPetals->normalMap = assets.createTexture("./assets/small-flower/petals-normals.png");
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION | ObjectEffects::GRASS_ANIMATION;
    flowerPetals->shadowCascadeLimit = 2;
    flowerPetals->isReference = true;
  });

  stage.add<Mesh>("lavender-stalk", [&](Mesh* lavenderStalk) {
    lavenderStalk->from(ObjLoader("./assets/lavender/stalk-model.obj"));
    lavenderStalk->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderStalk->shadowCascadeLimit = 2;
    lavenderStalk->isReference = true;
  });

  stage.add<Mesh>("lavender-flowers", [&](Mesh* lavenderFlowers) {
    lavenderFlowers->from(ObjLoader("./assets/lavender/flowers-model.obj"));
    lavenderFlowers->effects = ObjectEffects::GRASS_ANIMATION;
    lavenderFlowers->shadowCascadeLimit = 2;
    lavenderFlowers->isReference = true;
  });

  stage.add<Mesh>("lantern", [&](Mesh* lantern) {
    lantern->from(ObjLoader("./assets/lantern/model.obj"));
    lantern->texture = assets.createTexture("./assets/lantern/texture.png");
    lantern->normalMap = assets.createTexture("./assets/lantern/normals.png");
    lantern->isReference = true;
  });

  stage.addMultiple<Mesh, 10>([&](Mesh* lantern, int index) {
    Vec3f lanternPosition = getRandomGroundPosition() - Vec3f(0.0f, 5.0f, 0.0f);
    Vec3f lightPosition = lanternPosition + Vec3f(0.0f, 75.0f, 0.0f);

    lantern->from(stage.get<Mesh>("lantern"));
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

  stage.add<Plane>([&](Plane* plane) {
    float tileSize = 40.0f;

    plane->setSize(60, 60, tileSize, Vec2f(5.0f, 5.0f));
    plane->setPosition(Vec3f(0.0f));
    plane->texture = assets.createTexture("./assets/ground/grass-texture.png");
    plane->shadowCascadeLimit = 0;

    plane->displaceVertices([=](Vec3f& vertex, int x, int z) {
      float properX = x * tileSize - 1200.0f;
      float properZ = 1200.0f - z * tileSize;

      vertex.y += getGroundHeight(properX, properZ);
    });
  });

  stage.add<Skybox>([&](Skybox* skybox) {
    skybox->from(assets.createTexture("./assets/skybox/night.png"));
    skybox->setScale(5000.0f);

    skybox->onUpdate = [=](float dt) {
      skybox->setPosition(camera.position);
    };
  });

  addGrass();
  addRocks();
  addTrees();

  input.onMouseMotion([=](const SDL_MouseMotionEvent& event) {
    if (SDL_GetRelativeMouseMode()) {
      camera.orientation.x -= event.yrel / 1000.0f;
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
  float speedFactor = (input.isKeyHeld(Key::SHIFT) ? 70.0f : 20.0f);

  viewMatrix = Matrix4::rotate(camera.orientation * Vec3f(1.0f, -1.0f, 1.0f)) * Matrix4::translate(camera.position.invert());

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

  stage.add<Mesh>([&](Mesh* flowerStalk) {
    flowerStalk->from(stage.get<Mesh>("flower-stalk"));
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

  stage.add<Mesh>([&](Mesh* flowerPetals) {
    flowerPetals->from(stage.get<Mesh>("flower-petals"));
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

  stage.add<Mesh>([&](Mesh* stalk) {
    stalk->from(stage.get<Mesh>("lavender-stalk"));
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

  stage.add<Mesh>([&](Mesh* flowers) {
    flowers->from(stage.get<Mesh>("lavender-flowers"));
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

  stage.add<Mesh>([&](Mesh* sprout) {
    sprout->from(stage.get<Mesh>("sprout"));
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

  stage.addMultiple<Mesh, 5>([&](Mesh* seed, int index) {
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