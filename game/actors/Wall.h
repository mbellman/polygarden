#pragma once

#include <subsystem/entities/Actor.h>

class Wall : public Actor {
public:
  void onRegistered() override;
  void onInit() override;
};