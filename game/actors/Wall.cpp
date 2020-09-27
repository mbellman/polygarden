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
    wall->normalMap = Texture::use("./assets/wall/wall-normals.png");
  });

  stage->add<ReferenceMesh>("wall-wood", [](ReferenceMesh* wood) {
    wood->from(ObjLoader("./assets/wall/wood-model.obj"));
  });

  stage->add<ReferenceMesh>("wall-roof", [](ReferenceMesh* roof) {
    roof->from(ObjLoader("./assets/wall/roof-model.obj"));
    roof->texture = Texture::use("./assets/wall/roof-texture.png");
  });
}

void Wall::onInit() {
  stage->add<Instance>([&](Instance* wall) {
    wall->from(stage->get("wall"));

    addTransformable(wall);
  });

  stage->add<Instance>([&](Instance* wood) {
    wood->from(stage->get("wall-wood"));
    wood->setColor(Vec3f(1.0f, 0.9f, 0.8f));

    addTransformable(wood);
  });

  stage->add<Instance>([&](Instance* roof) {
    roof->from(stage->get("wall-roof"));

    addTransformable(roof);
  });
}