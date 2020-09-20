#pragma once

#include <subsystem/entities/Actor.h>
#include <subsystem/entities/Mesh.h>
#include <subsystem/Math.h>

class Wall : public Actor {
public:
  void onRegistered() override;
  void onInit() override;
};