#pragma once

#include <vector>

#include <subsystem/entities/Actor.h>
#include <subsystem/entities/Object.h>
#include <subsystem/Math.h>

class VisibleObjectFilter : public Actor {
public:
  void addObject(Object* object);
  void addObjects(const std::vector<Object*>& objects);
  void onUpdate(float dt) override;

private:
  std::vector<Object*> objects;
};