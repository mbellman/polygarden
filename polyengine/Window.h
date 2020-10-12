#pragma once

#include "SDL.h"
#include "Math.h"
#include "subsystem/AbstractVideoController.h"
#include "subsystem/AbstractGameController.h"
#include "subsystem/InputSystem.h"
#include "subsystem/Math.h"

class Window {
public:
  Window();
  ~Window();

  static Area<unsigned int> size;

  void open(const char* title, Region2d<unsigned int> region);
  void run();
  void setGameController(AbstractGameController* gameController);
  void setVideoController(AbstractVideoController* videoController);

private:
  bool didCloseWindow = false;
  SDL_Window* sdlWindow = nullptr;
  AbstractVideoController* videoController = nullptr;
  AbstractGameController* gameController = nullptr;

  void handleStats();
  void pollEvents();
};