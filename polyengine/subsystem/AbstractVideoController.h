#pragma once

#include "SDL.h"
#include "subsystem/Math.h"
#include "subsystem/AbstractScene.h"

class AbstractVideoController {
public:
  virtual ~AbstractVideoController() {};

  virtual void onDestroy() {};
  virtual void onInit(SDL_Window* sdlWindow, int width, int height) = 0;
  virtual void onRender(SDL_Window* sdlWindow) = 0;
  virtual void onSceneChange(AbstractScene* scene) = 0;
  virtual void onScreenSizeChange(int width, int height) {};
  virtual void setScene(AbstractScene* scene) final;
  virtual void toggleFullScreen(SDL_Window* sdlWindow) final;

protected:
  AbstractScene* scene = nullptr;
  Region2d<int> screenSize;
};