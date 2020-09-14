#pragma once

#include <vector>
#include <functional>
#include <string>
#include <map>
#include <set>
#include <type_traits>

#include "subsystem/entities/Entity.h"
#include "subsystem/entities/Light.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Actor.h"
#include "subsystem/HeapList.h"
#include "subsystem/Types.h"

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
  void add(Actor* actor);

  template<typename T>
  void add() {
    add<T>([](T* t) {});
  }

  template<typename T>
  void add(std::function<void(T*)> handler) {
    add<T>("__dummy__", handler);
  }

  template<typename T>
  void add(std::string name) {
    add<T>(name, [](T* t) {});
  }

  template<typename T>
  void add(std::string name, std::function<void(T*)> handler) {
    T* t = new T();

    if (std::is_base_of<Actor, T>::value) {
      // TODO: Determine why this ordering crashes for
      // regular entities, shadowcasters notwithstanding
      add(t);
      handler(t);
    } else {
      handler(t);
      add(t);
    }

    store.emplace(name, t);
  }

  template<typename T, unsigned int total>
  void addMultiple(std::function<void(T*, int)> handler) {
    for (unsigned int i = 0; i < total; i++) {
      add<T>([=](T* t) {
        handler(t, i);
      });
    }
  }

  template<typename T>
  T* get(std::string name) {
    return (T*)store.at(name);
  }

  const HeapList<Light>& getLights() const;
  const HeapList<Object>& getObjects() const;
  StageStats getStats() const;
  void onEntityAdded(Callback<Entity*> handler);
  void onEntityRemoved(Callback<Entity*> handler);
  void remove(Entity* entity);
  void update(float dt);

private:
  HeapList<Object> objects;
  HeapList<Light> lights;
  HeapList<Actor> actors;
  std::map<std::string, void*> store;
  std::set<std::size_t> registeredActorTypes;
  Callback<Entity*> entityAddedHandler = nullptr;
  Callback<Entity*> entityRemovedHandler = nullptr;

  bool isActorRegistered(Actor* actor);
  void removeExpiredEntities();
};