#include "subsystem/AbstractGameController.h"

AbstractGameController::~AbstractGameController() {
  for (auto* scene : sceneStack) {
    delete scene;
  }

  sceneStack.clear();
}

void AbstractGameController::changeScene(AbstractScene* scene) {
  if (activeScene == nullptr) {
    enterScene(scene);
  } else {
    delete activeScene;

    sceneStack.pop_back();
    sceneStack.push_back(scene);

    setActiveScene(scene);
  }
}

void AbstractGameController::enterScene(AbstractScene* scene) {
  sceneStack.push_back(scene);

  setActiveScene(scene);
}

void AbstractGameController::exitScene() {
  delete sceneStack[sceneStack.size() - 1];

  sceneStack.pop_back();

  setActiveScene(sceneStack.size() > 0 ? sceneStack[sceneStack.size() - 1] : nullptr);
}

AbstractScene* AbstractGameController::getActiveScene() {
  return activeScene;
}

void AbstractGameController::handleSceneChange(Callback<AbstractScene*> handler) {
  sceneChangeHandler = handler;
}

void AbstractGameController::setActiveScene(AbstractScene* scene) {
  activeScene = scene;

  if (sceneChangeHandler != nullptr) {
    sceneChangeHandler(scene);
  }

  if (!scene->isInitialized()) {
    scene->onInit();
  }
}

void AbstractGameController::update(float dt) {
  activeScene->update(dt);
}