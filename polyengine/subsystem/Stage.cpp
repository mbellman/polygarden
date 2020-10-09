#include <algorithm>
#include <typeinfo>

#include "subsystem/Stage.h"
#include "subsystem/entities/Instance.h"

Stage::~Stage() {
  objects.free();
  lights.free();
  actors.free();

  store.clear();
}

void Stage::add(Entity* entity) {
  saveEntity(entity);

  if (entityAddedHandler) {
    entityAddedHandler(entity);
  }
}

void Stage::add(Actor* actor) {
  saveActor(actor);

  actor->onAdded();
}

void Stage::saveActor(Actor* actor) {
  actor->setStage(this);

  if (!isActorRegistered(actor)) {
    actor->onRegistered();

    registeredActorTypes.insert(typeid(*actor).hash_code());
  }

  actors.push(actor);

  actor->onInit();
}

void Stage::saveEntity(Entity* entity) {
  if (entity->isOfType<Object>()) {
    objects.push((Object*)entity);
  } else if (entity->isOfType<Light>()) {
    lights.push((Light*)entity);
  }
}

const HeapList<Light>& Stage::getLights() const {
  return lights;
}

const HeapList<Object>& Stage::getObjects() const {
  return objects;
}

bool Stage::isActorRegistered(Actor* actor) {
  return registeredActorTypes.find(typeid(*actor).hash_code()) != registeredActorTypes.end();
}

void Stage::onEntityAdded(Callback<Entity*> handler) {
  entityAddedHandler = handler;
}

void Stage::onEntityRemoved(Callback<Entity*> handler) {
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

void Stage::update(float dt) {
  auto updateEntity = [=](Entity* entity) {
    if (entity->onUpdate) {
      entity->onUpdate(dt);
    }

    if (entity->lifetime > 0.0f) {
      entity->lifetime = std::max(entity->lifetime - dt, 0.0f);
    }
  };

  for (auto* actor : actors) {
    actor->update(dt);
  }

  for (unsigned int i = 0; i < objects.length(); i++) {
    updateEntity(objects[i]);
  }

  for (unsigned int i = 0; i < lights.length(); i++) {
    updateEntity(lights[i]);
  }

  removeExpiredEntities();

  for (auto* object : objects) {
    object->rehydrate();
  }
}