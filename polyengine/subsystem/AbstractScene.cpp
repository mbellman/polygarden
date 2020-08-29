#include <algorithm>
#include <functional>

#include "subsystem/AbstractScene.h"
#include "subsystem/RNG.h"

AbstractScene::AbstractScene() {
  RNG::seed();
}

void AbstractScene::delay(std::function<void()> callback, float timeout) {
  auto* record = new DelayRecord();

  record->callback = callback;
  record->timeout = timeout;
  record->creationTime = getRunningTime();

  delayRecords.push(record);
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

void AbstractScene::handleDelayRecords() {
  int i = 0;

  while (i < delayRecords.length()) {
    auto* record = delayRecords[i];

    if ((getRunningTime() - record->creationTime) >= record->timeout) {
      record->callback();
      delayRecords.remove(record);
    } else {
      i++;
    }
  }
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
  handleDelayRecords();

  auto updateEntity = [=](Entity* entity) {
    if (entity->onUpdate) {
      entity->onUpdate(dt);
    }

    if (entity->lifetime > 0.0f) {
      entity->lifetime = std::max(entity->lifetime - dt, 0.0f);
    }
  };

  for (auto* object : stage.getObjects()) {
    updateEntity(object);
  }

  for (auto* light : stage.getLights()) {
    updateEntity(light);
  }

  stage.removeExpiredEntities();

  runningTime += dt;
}