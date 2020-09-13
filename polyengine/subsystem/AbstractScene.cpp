#include <algorithm>
#include <functional>

#include "subsystem/AbstractScene.h"
#include "subsystem/RNG.h"

const Camera& AbstractScene::getCamera() const {
  return camera;
}

InputSystem& AbstractScene::getInputSystem() {
  return input;
}

float AbstractScene::getRunningTime() {
  return runningTime;
}

const Stage& AbstractScene::getStage() const {
  return stage;
}

std::function<float()> AbstractScene::getTimer() {
  float time = getRunningTime();

  return [=]() {
    return getRunningTime() - time;
  };
}

bool AbstractScene::isInitialized() {
  return runningTime > 0.0f;
}

void AbstractScene::onEntityAdded(EntityHandler handler) {
  stage.onEntityAdded(handler);
}

void AbstractScene::onEntityRemoved(EntityHandler handler) {
  stage.onEntityRemoved(handler);
}

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

  for (auto* object : stage.getObjects()) {
    object->rehydrate();
  }

  for (unsigned int i = 0; i < stage.getLights().length(); i++) {
    updateEntity(stage.getLights()[i]);
  }

  stage.removeExpiredEntities();

  runningTime += dt;
}