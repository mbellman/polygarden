#include <algorithm>

#include "Stats.h"
#include "SDL.h"

unsigned int Stats::getAverageFPS() {
  if (frameCounter == 0) {
    return 0;
  }

  unsigned int sum = 0;
  int totalSamples = std::min(120, (int)frameCounter);

  for (unsigned int i = 0; i < totalSamples; i++) {
    sum += fpsSamples[i];
  }

  return (unsigned int)(sum / (float)totalSamples);
}

unsigned int Stats::getFPS() {
  int delta = frame.end - frame.start;

  return delta == 0
    ? 1000
    : (int)(1000.0f / (frame.end - frame.start));
}

void Stats::trackFrameStart() {
  frame.start = SDL_GetTicks();
}

void Stats::trackFrameEnd() {
  frame.end = SDL_GetTicks();
  fpsSamples[frameCounter % 120] = getFPS();

  frameCounter++;
}