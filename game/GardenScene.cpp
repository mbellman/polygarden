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

void GardenScene::addGrass() {
  stage.add<Model>("grass", [](Model* grass) {
    grass->from(ObjLoader("./game/objects/grass/model.obj"));
    grass->effects = ObjectEffects::GRASS_ANIMATION;
    grass->canCastShadows = false;
    grass->isReference = true;
  });

  stage.addMultiple<Model, 10000>([&](Model* blade, int index) {
    float x = RNG::random(-1250.0f, 1250.0f);
    float z = RNG::random(-1250.0f, 1250.0f);

    Vec3f position = {
      x,
      getGroundHeight(1250.0f + x, -z + 1250.0f),
      z
    };

    blade->from(stage.get<Model>("grass"));
    blade->setScale(RNG::random(5.0f, 15.0f));
    blade->setPosition(position);
    blade->setColor(Vec3f(0.2f, 0.5f, 0.2f));
    blade->rotate(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
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
    float x = RNG::random(-1250.0f, 1250.0f);
    float z = RNG::random(-1250.0f, 1250.0f);

    Vec3f position = {
      x,
      getGroundHeight(1250.0f + x, -z + 1250.0f),
      z
    };

    rock->from(stage.get<Model>("rock"));
    rock->setPosition(position);
    rock->setScale(RNG::random(15.0f, 30.0f));
    rock->setOrientation(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
    rock->setColor(Vec3f(0.2f));
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

  stage.addMultiple<Model, 10>([&](Model* tree, int index) {
    float x = RNG::random(-1250.0f, 1250.0f);
    float z = RNG::random(-1250.0f, 1250.0f);

    Vec3f position(x, getGroundHeight(1250.0f + x, -z + 1250.0f), z);
    Vec3f orientation(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f);
    float scale = 40.0f;

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
    sprout->isReference = true;
  });

  stage.add<Model>("flower-stalk", [](Model* flowerStalk) {
    flowerStalk->from(ObjLoader("./game/objects/small-flower/stalk-model.obj"));
    flowerStalk->effects = ObjectEffects::GRASS_ANIMATION;
    flowerStalk->isReference = true;
  });
  
  stage.add<Model>("flower-petals", [](Model* flowerPetals) {
    flowerPetals->from(ObjLoader("./game/objects/small-flower/petals-model.obj"));
    flowerPetals->effects = ObjectEffects::TREE_ANIMATION | ObjectEffects::GRASS_ANIMATION;
    flowerPetals->isReference = true;
  });

  stage.add<Model>("lantern", [&](Model* lantern) {
    lantern->from(ObjLoader("./game/objects/lantern/model.obj"));
    lantern->texture = assets.createTexture("./game/objects/lantern/texture.png");
    lantern->normalMap = assets.createTexture("./game/objects/lantern/normals.png");
    lantern->isReference = true;
  });

  stage.addMultiple<Model, 10>([&](Model* lantern, int index) {
    float x = RNG::random() * 2500.0f - 1250.0f;
    float z = RNG::random() * 2500.0f - 1250.0f;
    float y = getGroundHeight(1250.0f + x, -z + 1250.0f) - 5.0f;

    lantern->from(stage.get<Model>("lantern"));
    lantern->setScale(100.0f);
    lantern->setPosition(Vec3f(x, y, z));

    stage.add<Light>([=](Light* light) {
      light->color = Vec3f(1.0f, 1.0f, 0.2f);
      light->position = Vec3f(x, y + 75.0f, z);
      light->radius = 750.0f;
      light->power = 4.0f;
    });
  });

  stage.add<Light>([](Light* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.5f, 1.0f);
    light->direction = Vec3f(-1.0f, -0.5f, 0.25f);
    light->canCastShadows = true;
  });

  stage.add<Light>([](auto* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(0.2f, 0.2f, 0.3f);
    light->direction = Vec3f(0.0f, -1.0f, 0.0f);
  });

  stage.add<Mesh>([&](Mesh* mesh) {
    mesh->setSize(250, 250, 10.0f, Vec2f(5.0f, 5.0f));
    mesh->setPosition(Vec3f(0.0f));
    mesh->texture = assets.createTexture("./game/objects/ground/grass-texture.png");
    mesh->canCastShadows = false;

    mesh->displace([=](Vec3f& vertex, int x, int z) {
      vertex.y += getGroundHeight(x * 10.0f, z * 10.0f);
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
  float scale = RNG::random(6.0f, 10.0f);

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

    sprout->from(stage.get<Model>("sprout"));
    sprout->setPosition(position);
    sprout->setOrientation(Vec3f(0.0f, RNG::random() * M_PI * 2.0f, 0.0f));
    sprout->color = Vec3f(0.25f, 1.0f, 0.5f);

    auto timer = getTimer();

    sprout->onUpdate = [=](float dt) {
      float t = timer() / 1.0f;

      if (t <= 1.0f) {
        sprout->setScale(Easing::bounceOut(t) * 5.0f);
      }
    };
  });
}

void GardenScene::throwSeeds() {
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