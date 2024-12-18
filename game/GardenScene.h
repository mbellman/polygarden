#pragma once

#include <map>
#include <string>

#include <PolyEngine.h>

class GardenScene : public AbstractScene {
public:
  void onInit() override;
  void onUpdate(float dt) override;

private:
  Vec3f velocity = Vec3f(0.0f);

  void addGrass();
  void addRocks();
  void addTrees();
  void spawnFlower(float x, float z);
  void spawnLavender(float x, float z);
  void spawnSprout(float x, float z);
  void throwSeeds();
};