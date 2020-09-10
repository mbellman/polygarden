#pragma once

#include <vector>
#include <functional>

#include "subsystem/Stage.h"
#include "subsystem/AssetCache.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/InputSystem.h"

class AbstractScene {
public:
  AbstractScene();
  virtual ~AbstractScene() {};

  const Camera& getCamera() const;
  virtual InputSystem& getInputSystem() final;
  float getRunningTime();
  virtual const Stage& getStage() const final;
  void onEntityAdded(EntityHandler handler);
  void onEntityRemoved(EntityHandler handler);
  virtual void onInit();
  virtual void onUpdate(float dt);
  virtual void update(float dt) final;

protected:
  Stage stage;
  AssetCache assets;
  InputSystem inputSystem;
  Camera camera;

  std::function<float()> getTimer();

private:
  float runningTime = 0.0f;
};