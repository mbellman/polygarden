#include "subsystem/AbstractGameController.h"

class GameController : public AbstractGameController {
public:
  void enterGardenScene();
  void onInit() override;
};