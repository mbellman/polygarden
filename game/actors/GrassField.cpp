#include <subsystem/entities/Mesh.h>
#include <subsystem/entities/Camera.h>
#include <subsystem/entities/Plane.h>
#include <subsystem/entities/ReferenceMesh.h>
#include <subsystem/entities/Instance.h>
#include <subsystem/Stage.h>
#include <subsystem/ObjLoader.h>
#include <subsystem/RNG.h>

#include "actors/GrassField.h"
#include "HeightMap.h"

void GrassField::onInit() {
  stage->add<Plane>([&](Plane* plane) {
    float tileSize = 40.0f;

    plane->setSize(60, 60, tileSize, Vec2f(5.0f, 5.0f));
    plane->setPosition(Vec3f(0.0f));
    plane->texture = Texture::use("./assets/ground/grass-texture.png");
    plane->shadowCascadeLimit = 0;

    plane->displaceVertices([=](Vec3f& vertex, int x, int z) {
      float properX = x * tileSize - 1200.0f;
      float properZ = 1200.0f - z * tileSize;

      vertex.y += HeightMap::getGroundHeight(properX, properZ);
    });
  });

  stage->addMultiple<Instance, 5000>([&](Instance* blade, int index) {
    blade->from(stage->get<Mesh>("grass"));
    blade->setPosition(HeightMap::getRandomGroundPosition());
    blade->setColor(Vec3f(0.2f, 0.5f, 0.2f));
    blade->rotate(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));
    blade->setScale(RNG::random(15.0f, 30.0f));
  });
}

void GrassField::onRegistered() {
  stage->add<ReferenceMesh>("grass", [&](ReferenceMesh* grass) {
    grass->from(ObjLoader("./assets/grass/model.obj"));
    grass->effects = ObjectEffects::GRASS_ANIMATION | ObjectEffects::TREE_ANIMATION;
    grass->shadowCascadeLimit = 3;
  });
}
