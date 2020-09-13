#include "GameController.h"
#include "GardenScene.h"

void GameController::enterGardenScene() {
  enterScene(new GardenScene());
}

void GameController::onInit() {
  enterGardenScene();
}