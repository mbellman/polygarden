#include <algorithm>
#include <functional>

#include "subsystem/AbstractScene.h"
#include "subsystem/RNG.h"

AbstractScene::AbstractScene() {
  RNG::seed();
}

const Camera& AbstractScene::getCamera() const {
  return camera;
}

InputSystem& AbstractScene::getInputSystem() {
  return inputSystem;
}

float AbstractScene::getRunningTime() {
  return runningTime;
}

const Stage& AbstractScene::getStage() const {
  return stage;
}

void AbstractScene::onEntityAdded(EntityHandler handler) {
  stage.onEntityAdded(handler);
}

void AbstractScene::onEntityRemoved(EntityHandler handler) {
  stage.onEntityRemoved(handler);
}

void AbstractScene::onInit() {}

void AbstractScene::onUpdate(float dt) {}

void AbstractScene::update(float dt) {
  onUpdate(dt);

  auto updateEntity = [=](Entity* entity) {
    if (entity->onUpdate) {
      entity->onUpdate(dt);
    }

    if (entity->lifetime > 0.0f) {
      entity->lifetime = std::max(entity->lifetime - dt, 0.0f);
    }
  };

  for (unsigned int i = 0; i < stage.getObjects().length(); i++) {
    updateEntity(stage.getObjects()[i]);
  }

  for (unsigned int i = 0; i < stage.getLights().length(); i++) {
    updateEntity(stage.getLights()[i]);
  }

  stage.removeExpiredEntities();

  runningTime += dt;
}