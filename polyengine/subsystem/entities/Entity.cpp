#include "subsystem/entities/Entity.h"

/**
 * Entity
 * ------
 */
Entity::Entity() {
  id = Entity::total++;
}

void Entity::expire() {
  lifetime = 0.0f;
}

bool Entity::isExpired() const {
  return lifetime == 0.0f;
}

int Entity::total = 0;