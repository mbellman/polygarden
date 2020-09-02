#include <algorithm>
#include <cstdio>

#include "subsystem/Stage.h"

Stage::~Stage() {
  objects.free();
  lights.free();
  entityMap.clear();
}

void Stage::add(Entity* entity) {
  if (entity->isOfType<Object>()) {
    objects.push((Object*)entity);
  } else if (entity->isOfType<Light>()) {
    lights.push((Light*)entity);
  }

  if (entityAddedHandler) {
    entityAddedHandler(entity);
  }
}

const HeapList<Light>& Stage::getLights() const {
  return lights;
}

const HeapList<Object>& Stage::getObjects() const {
  return objects;
}

StageStats Stage::getStats() const {
  StageStats stats;

  for (auto* object : objects) {
    stats.totalObjects++;
    stats.totalVertices += object->getReference()->getPolygons().size() * 3;

    if (object->isInstance()) {
      stats.totalInstances++;
    }
  }

  stats.totalLights = lights.length();

  for (auto* light : lights) {
    if (light->canCastShadows) {
      stats.totalShadowCasters++;
    }
  }

  return stats;
}

void Stage::onEntityAdded(EntityHandler handler) {
  entityAddedHandler = handler;
}

void Stage::onEntityRemoved(EntityHandler handler) {
  entityRemovedHandler = handler;
}

void Stage::remove(Entity* entity) {
  if (entityRemovedHandler) {
    entityRemovedHandler(entity);
  }

  if (entity->isOfType<Object>()) {
    objects.remove((Object*)entity);
  } else if (entity->isOfType<Light>()) {
    lights.remove((Light*)entity);
  }

  delete entity;
}

void Stage::removeExpiredEntities() {
  auto removeExpired = [=](auto& list) {
    unsigned int i = 0;

    while (i < list.length()) {
      Entity* entity = list[i];

      if (entity->isExpired()) {
        remove(entity);
      } else {
        i++;
      }
    }
  };

  removeExpired(objects);
  removeExpired(lights);
}
