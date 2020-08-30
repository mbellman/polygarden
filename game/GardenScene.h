#pragma once

#include <map>
#include <string>
#include <subsystem/AbstractScene.h>
#include <subsystem/Entities.h>

class GardenScene : public AbstractScene {
public:
  ~GardenScene();

  void onInit() override;
  void onUpdate(float dt) override;

private:
  // TODO: Move into AssetCache
  std::map<std::string, Model*> modelMap;

  void spawnFlower(float x, float z);
  void spawnSprout(float x, float z);
  void throwSeeds();
};