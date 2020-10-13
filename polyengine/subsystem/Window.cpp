#include <cstdio>
#include <cmath>

#include "subsystem/Window.h"
#include "subsystem/RNG.h"
#include "subsystem/AbstractScene.h"
#include "subsystem/PerformanceProfiler.h"
#include "SDL.h"

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
  char title[150];

  auto& profile = PerformanceProfiler::getProfile();

  sprintf_s(
    title,
    sizeof(title),
    "FPS: %u (%u), Objects: %u, Verts/Tris: %u/%u, Lights/Shadowcasters: %u/%u, Draw calls: %u, GPU Memory: %u/%u MB",
    profile.fps,
    profile.averageFps,
    profile.totalObjects,
    profile.totalVertices,
    profile.totalPolygons,
    profile.totalLights,
    profile.totalShadowCasters,
    profile.totalDrawCalls,
    profile.usedGpuMemory,
    profile.totalGpuMemory
  );

  SDL_SetWindowTitle(sdlWindow, title);
}

void Window::open(const char* title, Region2d<unsigned int> region) {
  Window::size = { region.width, region.height };
  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

  sdlWindow = SDL_CreateWindow(title, region.x, region.y, region.width, region.height, flags);
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
          Window::size = {
            (unsigned int)event.window.data1,
            (unsigned int)event.window.data2
          };

          videoController->onScreenSizeChange();
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

    PerformanceProfiler::trackFrameStart();

    pollEvents();
    gameController->getActiveScene()->update(dt);
    videoController->onRender(sdlWindow);

    PerformanceProfiler::trackFrameEnd();

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

  videoController->onInit(sdlWindow);

  // If the video controller is changed mid-scene, make sure
  // the new controller receives the existing scene instance
  if (gameController != nullptr && gameController->getActiveScene() != nullptr) {
    videoController->setScene(gameController->getActiveScene());
  }
}

Area<unsigned int> Window::size = { 0, 0 };