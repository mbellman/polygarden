#include <PolyEngine.h>

#include "actors/Rock.h"
#include "HeightMap.h"

void Rock::onRegistered() {
  stage->add<ReferenceMesh>("rock", [&](ReferenceMesh* rock) {
    Mesh* shadowLod = new Mesh();

    shadowLod->from(ObjLoader("./assets/rock-1/model-lod.obj"));

    rock->from(ObjLoader("./assets/rock-1/model.obj"));
    rock->texture = Texture::use("./assets/rock-1/texture.png");
    rock->normalMap = Texture::use("./assets/rock-1/normals.png");
    rock->shadowLod = shadowLod;
  });
}

void Rock::onInit() {
  stage->add<Instance>([&](Instance* rock) {
    rock->from(stage->get<Mesh>("rock"));
    rock->setPosition(HeightMap::getRandomGroundPosition());
    rock->setScale(RNG::random(15.0f, 30.0f));
    rock->setOrientation(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
  });
}