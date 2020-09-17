#include <cmath>

#include <subsystem/entities/Camera.h>

#include "actors/VisibleObjectFilter.h"

void VisibleObjectFilter::addObject(Object* object) {
  objects.push_back(object);
}

void VisibleObjectFilter::addObjects(const std::vector<Object*>& objects) {
  for (auto* object : objects) {
    addObject(object);
  }
}

void VisibleObjectFilter::onUpdate(float dt) {
  Matrix4 viewMatrix = (
    Matrix4::rotate(Camera::active->orientation * Vec3f(1.0f, -1.0f, 1.0f)) *
    Matrix4::translate(Camera::active->position.invert())
  );

  float frustumFactor = std::tanf(0.5f * Camera::active->fov * M_PI / 180.0f);

  for (auto* object : objects) {
    object->enableRenderingWhere([&](Object* object) {
      Vec3f relativePosition = viewMatrix * object->position;
      float frustumLimit = relativePosition.z * frustumFactor;

      return (
        relativePosition.z > 0.0f &&
        relativePosition.x > -frustumLimit &&
        relativePosition.x < frustumLimit &&
        relativePosition.y > -frustumLimit &&
        relativePosition.y < frustumLimit
      );
    });
  }
}