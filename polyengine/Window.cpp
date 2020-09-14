#include <cstdio>
#include <cmath>

#include "Window.h"
#include "SDL.h"
#include "Stats.h"
#include "subsystem/RNG.h"
#include "subsystem/AbstractScene.h"

Window::Window() {
  SDL_Init(SDL_INIT_EVERYTHING);

  RNG::seed();
}

Window::~Window() {
  videoController->onDestroy();

  delete videoController;

  SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}

void Window::handleStats() {
  char title[100];

  auto stageStats = gameController->getActiveScene()->getStage().getStats();

  sprintf_s(
    title,
    sizeof(title),
    "FPS: %d (%d), Objects: %d (%d instances), Vertices: %d, Lights: %d, Shadowcasters: %d",
    stats.getFPS(),
    stats.getAverageFPS(),
    stageStats.totalObjects,
    stageStats.totalInstances,
    stageStats.totalVertices,
    stageStats.totalLights,
    stageStats.totalShadowCasters
  );

  SDL_SetWindowTitle(sdlWindow, title);
}

void Window::open(const char* title, Region2d<int> region) {
  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

  sdlWindow = SDL_CreateWindow(title, region.x, region.y, region.width, region.height, flags);

  size.width = region.width;
  size.height = region.height;
}

void Window::pollEvents() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        didCloseWindow = true;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          size.width = event.window.data1;
          size.height = event.window.data2;

          videoController->onScreenSizeChange(size.width, size.height);
        }

        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_f) {
          videoController->toggleFullScreen(sdlWindow);
        }

        break;
      default:
        break;
    }

    gameController->getActiveScene()->getInputSystem().handleEvent(event);
  }
}

void Window::run() {
  gameController->handleSceneChange([&](AbstractScene* scene) {
    videoController->setScene(scene);
  });

  gameController->onInit();

  int lastTick = SDL_GetTicks();

  while (!didCloseWindow) {
    float dt = (SDL_GetTicks() - lastTick) / 1000.0f;

    lastTick = SDL_GetTicks();

    stats.trackFrameStart();
    pollEvents();
    gameController->getActiveScene()->update(dt);
    videoController->onRender(sdlWindow);
    stats.trackFrameEnd();
    handleStats();

    SDL_Delay(1);
  }
}

void Window::setGameController(AbstractGameController* gameController) {
  this->gameController = gameController;
}

void Window::setVideoController(AbstractVideoController* videoController) {
  if (this->videoController != nullptr) {
    this->videoController->onDestroy();

    delete this->videoController;
  }

  this->videoController = videoController;

  videoController->onInit(sdlWindow, size.width, size.height);

  // If the video controller is changed mid-scene, make sure
  // the new controller receives the existing scene instance
  if (gameController != nullptr && gameController->getActiveScene() != nullptr) {
    videoController->setScene(gameController->getActiveScene());
  }
}