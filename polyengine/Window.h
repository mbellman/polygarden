#pragma once

#include "SDL.h"
#include "Math.h"
#include "subsystem/AbstractVideoController.h"
#include "subsystem/AbstractGameController.h"
#include "subsystem/InputSystem.h"
#include "subsystem/Math.h"
#include "Stats.h"

class Window {
public:
  Window();
  ~Window();

  void open(const char* title, Region2d<int> region);
  void run();
  void setGameController(AbstractGameController* gameController);
  void setVideoController(AbstractVideoController* videoController);

private:
  bool didCloseWindow = false;
  SDL_Window* sdlWindow = nullptr;
  Region2d<int> size;
  AbstractVideoController* videoController = nullptr;
  AbstractGameController* gameController = nullptr;
  Stats stats;

  void handleStats();
  void pollEvents();
};