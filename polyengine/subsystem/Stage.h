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
    bool isEntity = std::is_base_of<Entity, T>::value;
    bool isActor = std::is_base_of<Actor, T>::value;

    T* t = new T();

    if (isEntity) {
      saveEntity((Entity*)t);
    } else if (isActor) {
      saveActor((Actor*)t);
    }

    handler(t);

    store.emplace(name, t);

    if (isEntity && entityAddedHandler) {
      entityAddedHandler((Entity*)t);
    } else if (isActor) {
      ((Actor*)t)->onAdded();
    }
  }

  template<typename T, unsigned int total>
  void addMultiple(std::function<void(T*, int)> handler) {
    for (unsigned int i = 0; i < total; i++) {
      add<T>([=](T* t) {
        handler(t, i);
      });
    }
  }

  template<typename T = Object>
  T* get(std::string name) {
    return (T*)store.at(name);
  }

  const HeapList<Light>& getLights() const;
  const HeapList<Object>& getObjects() const;
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

  void saveActor(Actor* actor);
  void saveEntity(Entity* entity);
  bool isActorRegistered(Actor* actor);
  void removeExpiredEntities();
};