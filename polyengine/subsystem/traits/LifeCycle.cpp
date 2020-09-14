#include "subsystem/traits/LifeCycle.h"

std::function<float()> LifeCycle::createTimer() {
  float time = getRunningTime();

  return [=]() {
    return getRunningTime() - time;
  };
}

float LifeCycle::getRunningTime() {
  return runningTime;
}

bool LifeCycle::isInitialized() {
  return runningTime > 0.0f;
}

void LifeCycle::update(float dt) {
  runningTime += dt;

  onUpdate(dt);
}