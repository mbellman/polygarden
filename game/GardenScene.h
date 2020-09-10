#pragma once

#include <map>
#include <string>
#include <subsystem/AbstractScene.h>
#include <subsystem/Entities.h>

class GardenScene : public AbstractScene {
public:
  void onInit() override;
  void onUpdate(float dt) override;

private:
  Vec3f velocity = Vec3f(0.0f);
  Matrix4 viewMatrix;

  void addGrass();
  void addRocks();
  void addTrees();
  void spawnFlower(float x, float z);
  void spawnLavender(float x, float z);
  void spawnSprout(float x, float z);
  void throwSeeds();
};