#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/ObjLoader.h"

class Model : public Object {
public:
  void from(const ObjLoader& loader);
  void from(Model* reference);

private:
  void buildTexturedModel(const ObjLoader& loader);
  void buildUntexturedModel(const ObjLoader& loader);
};