#pragma once

#include <vector>

#include "subsystem/AbstractScene.h"
#include "subsystem/Types.h"

class AbstractGameController {
public:
  virtual ~AbstractGameController();

  virtual AbstractScene* getActiveScene() final;
  virtual void handleSceneChange(Callback<AbstractScene*> handler) final;
  virtual void onInit() = 0;

protected:
  void changeScene(AbstractScene* scene);
  void enterScene(AbstractScene* scene);
  void exitScene();

private:
  std::vector<AbstractScene*> sceneStack;
  AbstractScene* activeScene = nullptr;
  Callback<AbstractScene*> sceneChangeHandler = nullptr;

  void setActiveScene(AbstractScene* scene);
  void pollEvents();
};