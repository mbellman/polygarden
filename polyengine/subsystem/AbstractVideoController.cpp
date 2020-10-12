#include "SDL.h"
#include "subsystem/AbstractVideoController.h"

void AbstractVideoController::setScene(AbstractScene* scene) {
  this->scene = scene;

  onSceneChange(scene);
}

// TODO: Move to Window
void AbstractVideoController::toggleFullScreen(SDL_Window* sdlWindow) {
  int flags = SDL_GetWindowFlags(sdlWindow);
  bool isFullScreen = flags & SDL_WINDOW_FULLSCREEN;

  SDL_DisplayMode display;

  SDL_GetDesktopDisplayMode(0, &display);

  int fullScreenFlags = isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN;
  int updatedWidth = isFullScreen ? 1200 : display.w;
  int updatedHeight = isFullScreen ? 720 : display.h;

  SDL_SetWindowFullscreen(sdlWindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN);
  SDL_SetWindowSize(sdlWindow, updatedWidth, updatedHeight);

  onScreenSizeChange();
}
