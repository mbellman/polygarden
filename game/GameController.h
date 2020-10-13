#pragma once

#include <PolyEngine.h>

class GameController : public AbstractGameController {
public:
  void enterGardenScene();
  void onInit() override;
};