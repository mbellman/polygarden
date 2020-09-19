#include "subsystem/entities/Entity.h"
#include "subsystem/entities/Camera.h"

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

float Entity::getLocalDistance() const {
  return Camera::active == nullptr ? 0.0f : (Camera::active->position - position).magnitude();
}

Vec3f Entity::getLocalPosition() const {
  return Camera::active == nullptr ? position : Camera::active->getViewMatrix() * position;
}

bool Entity::isExpired() const {
  return lifetime == 0.0f;
}

int Entity::total = 0;