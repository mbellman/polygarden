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
  std::map<std::string, Model*> modelMap;
  std::map<std::string, ObjLoader*> objLoaderMap;

  void spawnFlower(float x, float z);
  void spawnSprout(float x, float z);
  void throwSeeds();
};