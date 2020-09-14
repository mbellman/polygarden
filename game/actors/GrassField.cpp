#include <subsystem/entities/Mesh.h>
#include <subsystem/entities/Camera.h>
#include <subsystem/Stage.h>
#include <subsystem/ObjLoader.h>
#include <subsystem/RNG.h>

#include "actors/GrassField.h"
#include "HeightMap.h"

void GrassField::onInit() {
  printf("GrassField onInit!\n");

  stage->addMultiple<Mesh, 10000>([&](Mesh* blade, int index) {
    blade->from(stage->get<Mesh>("grass"));
    blade->setScale(RNG::random(5.0f, 15.0f));
    blade->setPosition(HeightMap::getRandomGroundPosition());
    blade->setColor(Vec3f(0.2f, 0.5f, 0.2f));
    blade->rotate(Vec3f(0.0f, RNG::random(0.0f, M_PI * 2.0f), 0.0f));

    blade->onUpdate = [=](float dt) {
      Vec3f relativeBladePosition = viewMatrix * blade->position;

      if (relativeBladePosition.z > 0.0f && relativeBladePosition.z < 1250.0f) {
        blade->enable();
      } else {
        blade->disable();
      }
    };
  });
}

void GrassField::onRegistered() {
  printf("GrassField onRegistered!\n");

  stage->add<Mesh>("grass", [&](Mesh* grass) {
    grass->from(ObjLoader("./assets/grass/model.obj"));
    grass->effects = ObjectEffects::GRASS_ANIMATION;
    grass->shadowCascadeLimit = 0;
    grass->isReference = true;
  });
}

void GrassField::onUpdate(float dt) {
  viewMatrix = Matrix4::rotate(Camera::active->orientation * Vec3f(1.0f, -1.0f, 1.0f)) * Matrix4::translate(Camera::active->position.invert());
}