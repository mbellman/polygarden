#pragma once

#include <vector>
#include <functional>

#include "subsystem/Entities.h"
#include "subsystem/HeapList.h"

typedef std::function<void(Entity*)> EntityHandler;

struct StageStats {
  unsigned int totalObjects = 0;
  unsigned int totalInstances = 0;
  unsigned int totalVertices = 0;
  unsigned int totalLights = 0;
  unsigned int totalShadowCasters = 0;
};

class Stage {
public:
  ~Stage();

  void add(Entity* entity);

  template<typename T>
  void add(std::function<void(T*)> handler) {
    T* entity = new T();

    handler(entity);
    add(entity);
  }

  template<typename T, unsigned int total>
  void addMultiple(std::function<void(T*, int)> handler) {
    for (int i = 0; i < total; i++) {
      add<T>([=](T* entity) {
        handler(entity, i);
      });
    }
  }

  const HeapList<Light>& getLights() const;
  const HeapList<Object>& getObjects() const;
  StageStats getStats() const;
  void onEntityAdded(EntityHandler handler);
  void onEntityRemoved(EntityHandler handler);
  void remove(Entity* entity);
  void removeExpiredEntities();

private:
  HeapList<Object> objects;
  HeapList<Light> lights;
  EntityHandler entityAddedHandler = nullptr;
  EntityHandler entityRemovedHandler = nullptr;
};