#include <cmath>
#include <subsystem/Entities.h>

#include "GardenScene.h"

static float getMapHeight(float x, float z) {
  float fx = M_PI / 150.0f * x;
  float fz = M_PI / 150.0f * z;

  return (
    5.0f * (sinf(fx) + cosf(fz)) +
    20.0f * sinf(fx * 0.3f) + 10.0f * cosf(fz * 0.2f)
  );
};

void GardenScene::onInit() {
  stage.add<Light>([](auto* light) {
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
    mesh->setPosition({ 0.0f, -50.0f, 0.0f });
    mesh->texture = assets.createTexture("./game/grass-texture.png");

    mesh->displace([=](Vec3f& vertex, int x, int z) {
      vertex.y += getMapHeight(x * 10.0f, z * 10.0f);
    });
  });

  stage.add<Cube>([](Cube* cube) {
    cube->setScale({ 50.0f, 200.0f, 50.0f });
    cube->setPosition({ 0.0f, -10.0f, 250.0f });
    cube->setColor({ 1.0f, 0.2f, 0.5f });
  });

  stage.add<Skybox>([=](Skybox* skybox) {
    skybox->from(assets.createTexture("./game/dummy-skybox.png"));
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
  Vec3f xz = Vec3f(1.0f, 0.0f, 1.0f);
  float speedFactor = dt * (inputSystem.isKeyHeld(Key::SHIFT) ? 200.0f : 100.0f);

  if (inputSystem.isKeyHeld(Key::W)) {
    camera.position += camera.getDirection() * xz * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::A)) {
    camera.position += camera.getLeftDirection() * xz * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::S)) {
    camera.position -= camera.getDirection() * xz * speedFactor;
  }

  if (inputSystem.isKeyHeld(Key::D)) {
    camera.position += camera.getRightDirection() * xz * speedFactor;
  }

  camera.position.y = 10.0f + getMapHeight(1250.0f + camera.position.x, -camera.position.z + 1250.0f);
}