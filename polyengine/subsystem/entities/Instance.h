#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/entities/Mesh.h"

// TODO: Investigate decoupling Instance from Object, separating
// transformation methods, removing superfluous class members
class Instance : public Object {
public:
  ~Instance();

  void from(Object* reference);
  void rehydrate() override {};
};