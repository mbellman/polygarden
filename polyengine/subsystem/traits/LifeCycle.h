#pragma once

#include <functional>

class LifeCycle {
public:
  virtual ~LifeCycle() {};

  std::function<float()> createTimer();
  float getRunningTime();
  virtual bool isInitialized() final;
  virtual void onDestroy() {};
  virtual void onInit() {};
  virtual void onUpdate(float dt) {};
  virtual void update(float dt) final;

private:
  float runningTime = 0.0f;
};