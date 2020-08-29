#pragma once

#include <vector>
#include <functional>

#include "subsystem/Stage.h"
#include "subsystem/AssetCache.h"
#include "subsystem/Entities.h"
#include "subsystem/InputSystem.h"
#include "subsystem/HeapList.h"

struct DelayRecord {
  float creationTime;
  float timeout;
  std::function<void()> callback;
};

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

  void delay(std::function<void()> callback, float timeout);

private:
  float runningTime = 0.0f;
  HeapList<DelayRecord> delayRecords;

  void handleDelayRecords();
};