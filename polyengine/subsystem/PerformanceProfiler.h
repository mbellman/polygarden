#pragma once

#include "subsystem/Math.h"
#include "subsystem/entities/Object.h"
#include "subsystem/entities/Light.h"

struct PerformanceProfile {
  unsigned int fps = 0;
  unsigned int averageFps = 0;
  unsigned int totalObjects = 0;
  unsigned int totalVertices = 0;
  unsigned int totalPolygons = 0;
  unsigned int totalLights = 0;
  unsigned int totalShadowCasters = 0;
  unsigned int totalDrawCalls = 0;
  unsigned int totalGpuMemory = 0;
  unsigned int usedGpuMemory = 0;
};

class PerformanceProfiler {
public:
  static unsigned int getCurrentFrame();
  static const PerformanceProfile& getProfile();
  static void trackDrawCall();
  static void trackFrameEnd();
  static void trackFrameStart();
  static void trackGpuMemory(unsigned int totalMemory, unsigned int usedMemory);
  static void trackLight(const Light* light);
  static void trackObject(const Object* object, unsigned int totalRenderableInstances);

private:
  static PerformanceProfile profile;
  static Range<int> frame;
  static unsigned int currentFrame;
  static unsigned int fpsSamples[120];

  static unsigned int getAverageFps();
  static unsigned int getFps();
  static void resetProfile();
};