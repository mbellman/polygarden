#pragma once

#include <functional>

#include "subsystem/Math.h"

struct Entity {
  Entity();
  virtual ~Entity() {};

  static int total;
  int id;
  Vec3f position;
  Vec3f orientation;
  float lifetime = -1.0f;
  std::function<void(float)> onUpdate = nullptr;

  void expire();
  float getLocalDistance() const;
  Vec3f getLocalPosition() const;
  bool isExpired() const;

  template<typename T>
  bool isOfType() {
    return dynamic_cast<T*>(this) != 0;
  }
};