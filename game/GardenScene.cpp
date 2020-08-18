#include <subsystem/Entities.h>

#include "GardenScene.h"

void GardenScene::onInit() {
  stage.add<Light>([](auto* light) {
    light->type = Light::LightType::DIRECTIONAL;
    light->color = Vec3f(1.0f, 1.0f, 0.5f);
    light->direction = Vec3f(1.0f, -0.5f, 1.0f);
    light->canCastShadows = true;
  });

  stage.add<Mesh>([](Mesh* mesh) {
    mesh->create(4, 4, 500.0f);
    mesh->setColor(Vec3f(0.5f));
    mesh->setPosition({ 0.0f, -50.0f, 0.0f });
  });

  stage.add<Cube>([](Cube* cube) {
    cube->setScale(50.0f);
    cube->setPosition({ 0.0f, -10.0f, 250.0f });
    cube->setColor({ 1.0f, 0.2f, 0.5f });
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
  if (inputSystem.isKeyHeld(Key::W)) {
    camera.position += camera.getDirection() * 100.0f * dt;
  }

  if (inputSystem.isKeyHeld(Key::A)) {
    camera.position += camera.getLeftDirection() * 100.0f * dt;
  }

  if (inputSystem.isKeyHeld(Key::S)) {
    camera.position -= camera.getDirection() * 100.0f * dt;
  }

  if (inputSystem.isKeyHeld(Key::D)) {
    camera.position += camera.getRightDirection() * 100.0f * dt;
  }
}