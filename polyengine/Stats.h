#pragma once

#include "SDL.h"
#include "subsystem/Math.h"

typedef int SDL_Tick;

class Stats {
public:
  unsigned int getAverageFPS();
  unsigned int getFPS();
  void trackFrameStart();
  void trackFrameEnd();

private:
  unsigned int frameCounter = 0;
  Range<SDL_Tick> frame = { 0, 0 };
  unsigned int fpsSamples[120];
};