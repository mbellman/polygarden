#pragma once

#include "subsystem/entities/Object.h"
#include "subsystem/ObjLoader.h"

class Mesh : public Object {
public:
  void from(const ObjLoader& loader);

private:
  void buildTexturedMesh(const ObjLoader& loader);
  void buildUntexturedMesh(const ObjLoader& loader);
};