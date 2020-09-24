#include "subsystem/entities/Actor.h"

Actor::~Actor() {
  positionables.clear();
  scalables.clear();
  orientables.clear();
}

void Actor::addPositionable(Positionable* positionable) {
  positionables.push_back(positionable);
}

void Actor::addTransformable(Transformable* transformable) {
  addPositionable(transformable);

  orientables.push_back(transformable);
  scalables.push_back(transformable);
}

void Actor::setOrientation(const Vec3f& orientation) {
  for (auto* orientable : orientables) {
    orientable->setOrientation(orientation);
  }
}

void Actor::setPosition(const Vec3f& position) {
  for (auto* positionable : positionables) {
    positionable->setPosition(position);
  }
}

void Actor::setScale(const Vec3f& scale) {
  for (auto* scalable : scalables) {
    scalable->setScale(scale);
  }
}

void Actor::setStage(Stage* stage) {
  this->stage = stage;
}