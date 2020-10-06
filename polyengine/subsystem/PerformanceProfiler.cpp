#include <algorithm>

#include "subsystem/PerformanceProfiler.h"
#include "SDL.h"

unsigned int PerformanceProfiler::getAverageFps() {
  unsigned int samples = std::min(120, int(currentFrame));
  unsigned int sum = 0;

  for (unsigned int i = 0; i < samples; i++) {
    sum += fpsSamples[i];
  }

  return unsigned int(sum / float(samples));
}

unsigned int PerformanceProfiler::getCurrentFrame() {
  return currentFrame;
}

unsigned int PerformanceProfiler::getFps() {
  return unsigned int(1000.0f / float(frame.end - frame.start));
}

const PerformanceProfile& PerformanceProfiler::getProfile() {
  return profile;
}

void PerformanceProfiler::resetProfile() {
  profile.totalObjects = 0;
  profile.totalVertices = 0;
  profile.totalPolygons = 0;
  profile.totalLights = 0;
  profile.totalShadowCasters = 0;
  profile.totalDrawCalls = 0;
  profile.totalGpuMemory = 0;
  profile.usedGpuMemory = 0;
}

void PerformanceProfiler::trackDrawCall() {
  profile.totalDrawCalls++;
}

void PerformanceProfiler::trackFrameEnd() {
  frame.end = SDL_GetTicks();

  unsigned int fps = getFps();

  fpsSamples[currentFrame++ % 120] = fps;

  profile.fps = fps;
  profile.averageFps = getAverageFps();
}

void PerformanceProfiler::trackFrameStart() {
  resetProfile();

  frame.start = SDL_GetTicks();
}

void PerformanceProfiler::trackGpuMemory(unsigned int totalMemory, unsigned int usedMemory) {
  profile.totalGpuMemory = totalMemory;
  profile.usedGpuMemory = usedMemory;
}

void PerformanceProfiler::trackLight(const Light* light) {
  profile.totalLights++;

  if (light->canCastShadows) {
    profile.totalShadowCasters++;
  }
}

void PerformanceProfiler::trackObject(const Object* object, unsigned int totalRenderableInstances) {
  profile.totalObjects += totalRenderableInstances;
  profile.totalVertices += object->getVertices().size() * totalRenderableInstances;
  profile.totalPolygons += object->getPolygons().size() * totalRenderableInstances;
}

PerformanceProfile PerformanceProfiler::profile;
Range<int> PerformanceProfiler::frame;
unsigned int PerformanceProfiler::currentFrame = 0;
unsigned int PerformanceProfiler::fpsSamples[120];