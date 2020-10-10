#pragma once

#include <subsystem/entities/Actor.h>

class Rock : public Actor {
public:
  void onRegistered() override;
  void onInit() override;
};