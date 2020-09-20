#include <subsystem/Stage.h>
#include <subsystem/entities/ReferenceMesh.h>
#include <subsystem/entities/Instance.h>
#include <subsystem/ObjLoader.h>
#include <subsystem/Texture.h>

#include "actors/Wall.h"
#include "HeightMap.h"

void Wall::onRegistered() {
  stage->add<ReferenceMesh>("wall", [](ReferenceMesh* wall) {
    wall->from(ObjLoader("./assets/wall/wall-model.obj"));
    wall->texture = Texture::use("./assets/wall/wall-texture.png");
    wall->normalMap = Texture::use("./assets/wall/normals.png");
  });
}

void Wall::onInit() {
  // Left wall
  stage->addMultiple<Instance, 7>([&](Instance* wall, int index) {
    wall->from(stage->get("wall"));
    wall->setScale(62.0f);
    wall->setPosition(Vec3f(-1225.0f, -60.0f, -1050.0f + index * 350.0f));
  });

  // Front wall
  stage->addMultiple<Instance, 7>([&](Instance* wall, int index) {
    wall->from(stage->get("wall"));
    wall->setScale(62.0f);
    wall->setPosition(Vec3f(-1050.0f + index * 350.0f, -60.0f, 1225.0f));
    wall->setOrientation(Vec3f(0.0f, M_PI * 0.5f, 0.0f));
  });

  // Right wall
  stage->addMultiple<Instance, 7>([&](Instance* wall, int index) {
    wall->from(stage->get("wall"));
    wall->setScale(62.0f);
    wall->setPosition(Vec3f(1225.0f, -60.0f, -1050.0f + index * 350.0f));
  });

  // Back wall
  stage->addMultiple<Instance, 7>([&](Instance* wall, int index) {
    wall->from(stage->get("wall"));
    wall->setScale(62.0f);
    wall->setPosition(Vec3f(-1050.0f + index * 350.0f, -60.0f, -1225.0f));
    wall->setOrientation(Vec3f(0.0f, M_PI * 0.5f, 0.0f));
  });
}