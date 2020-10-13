#pragma once

#include <PolyEngine.h>

class Rock : public Actor {
public:
  void onRegistered() override;
  void onInit() override;
};