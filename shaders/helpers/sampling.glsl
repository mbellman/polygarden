const vec2 RADIAL_SAMPLE_OFFSETS_6[6] = vec2[6](
                     vec2(0.0, -1.0),
  vec2(-1.0, -0.33),                  vec2(1.0, -0.33),
  vec2(-1.0, 0.33),                   vec2(1.0, 0.33),
                     vec2(0.0, 1.0)
);

const vec2 RADIAL_SAMPLE_OFFSETS_8[8] = vec2[8](
  vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.7),
  vec2(-1.0, 0.0),                   vec2(1.0, 0.0),
  vec2(-0.7, 0.7),  vec2(0.0, 1.0),  vec2(0.7, 0.7)
);

const vec2 RADIAL_SAMPLE_OFFSETS_9[9] = vec2[9](
  vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.7),
  vec2(-1.0, 0.0),  vec2(0.0),       vec2(1.0, 0.0),
  vec2(-0.7, 0.7),  vec2(0.0, 1.0),  vec2(0.7, 0.7)
);

const vec2 CROSS_SAMPLE_OFFSETS[4] = vec2[4](
                   vec2(0.0, -1.0),
  vec2(-1.0, 0.0),                  vec2(1.0, 0.0),
                   vec2(0.0, 1.0)
);

const vec3 CUBE_SAMPLE_OFFSETS[7] = vec3[7](
  vec3(0.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0)
);

const float DITHERING_KERNEL[16] = float[16](
  0.0,    0.5,    0.125,  0.625,
  0.75,   0.22,   0.875,  0.375,
  0.1875, 0.6875, 0.0625, 0.5625,
  0.9375, 0.4375, 0.8125, 0.3125
);

int getSampleGridIndex(vec2 fragment, vec2 screenSize, int gridSize) {
  int yOffset = gridSize * (int(fragment.y * screenSize.y) % gridSize);
  int xOffset = int(fragment.x * screenSize.x) % gridSize;

  return yOffset + xOffset;
}

vec2 getRadialOffset(vec2 fragment, vec2 screenSize) {
  return RADIAL_SAMPLE_OFFSETS_9[getSampleGridIndex(fragment, screenSize, 3)];
}

float getDitheringFactor(vec2 fragment, vec2 screenSize) {
  return DITHERING_KERNEL[getSampleGridIndex(fragment, screenSize, 4)];
}