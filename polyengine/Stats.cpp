#include "Stats.h"
#include "SDL.h"

unsigned int Stats::getAverageFPS() {
  unsigned int sum = 0;

  for (unsigned int i = 0; i < 60; i++) {
    sum += fpsSamples[i];
  }

  return (unsigned int)(sum / 60.0f);
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
  fpsSamples[frameCounter % 60] = getFPS();

  frameCounter++;
}