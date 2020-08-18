#pragma once

#include <subsystem/AbstractScene.h>

class GardenScene : public AbstractScene {
public:
  void onInit() override;
  void onUpdate(float dt) override;
};