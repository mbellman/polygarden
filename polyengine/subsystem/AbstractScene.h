#pragma once

#include <vector>
#include <functional>

#include "subsystem/Stage.h"
#include "subsystem/AssetCache.h"
#include "subsystem/entities/Entity.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/InputSystem.h"
#include "subsystem/Types.h"

class AbstractScene {
public:
  virtual ~AbstractScene() {};

  const Camera& getCamera() const;
  virtual InputSystem& getInputSystem() final;
  float getRunningTime();
  virtual const Stage& getStage() const final;
  bool isInitialized();
  void onEntityAdded(Callback<Entity*> handler);
  void onEntityRemoved(Callback<Entity*> handler);
  virtual void onInit() {};
  virtual void onUpdate(float dt) {};
  virtual void update(float dt) final;

protected:
  Stage stage;
  AssetCache assets;
  InputSystem input;
  Camera camera;

  std::function<float()> getTimer();

private:
  float runningTime = 0.0f;
};