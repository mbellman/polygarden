#pragma once

#include <PolyEngine.h>

class Wall : public Actor {
public:
  void onRegistered() override;
  void onInit() override;
};