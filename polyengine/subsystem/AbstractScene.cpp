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

const Stage& AbstractScene::getStage() const {
  return stage;
}

void AbstractScene::onEntityAdded(Callback<Entity*> handler) {
  stage.onEntityAdded(handler);
}

void AbstractScene::onEntityRemoved(Callback<Entity*> handler) {
  stage.onEntityRemoved(handler);
}

void AbstractScene::onUpdate(float dt) {
  stage.update(dt);
}