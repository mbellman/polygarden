#include <cmath>

#include <PolyEngine.h>

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
  Matrix4 view = Camera::active->getViewMatrix();
  float frustumFactor = std::tanf(0.5f * Camera::active->fov * M_PI / 180.0f);

  for (auto* object : objects) {
    object->enableRenderingWhere([&](Object* object) {
      Vec3f localPosition = view * object->position;
      float frustumLimit = 25.0f + localPosition.z * frustumFactor;

      return (
        localPosition.z > 0.0f &&
        localPosition.x > -frustumLimit &&
        localPosition.x < frustumLimit &&
        localPosition.y > -frustumLimit &&
        localPosition.y < frustumLimit
      );
    });
  }
}