const vec2 RADIAL_SAMPLE_OFFSETS_8[8] = vec2[](
  vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.7),
  vec2(-1.0, 0.0),                   vec2(1.0, 0.0),
  vec2(-0.7, 0.7),  vec2(0.0, 1.0),  vec2(0.7, 0.7)
);

const vec2 RADIAL_SAMPLE_OFFSETS_9[9] = vec2[](
  vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.7),
  vec2(-1.0, 0.0),  vec2(0.0),       vec2(1.0, 0.0),
  vec2(-0.7, 0.7),  vec2(0.0, 1.0),  vec2(0.7, 0.7)
);

const vec2 CROSS_SAMPLE_OFFSETS[4] = vec2[](
                   vec2(0.0, -1.0),
  vec2(-1.0, 0.0),                  vec2(1.0, 0.0),
                   vec2(0.0, 1.0)
);

const vec2 DIAMOND_SAMPLE_OFFSETS[4] = vec2[](
  vec2(-1.0, -1.0),                 vec2(1.0, -1.0),

  vec2(-1.0, 1.0),                  vec2(1.0, 1.0)
);

const vec3 CUBE_SAMPLE_OFFSETS[7] = vec3[](
  vec3(0.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0)
);

const float DITHERING_KERNEL[16] = float[](
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

float getDitheringFactor(vec2 fragment, vec2 screenSize) {
  return DITHERING_KERNEL[getSampleGridIndex(fragment, screenSize, 4)];
}

vec3 getWeightedSpread(sampler2D image, vec2 uv, int spread, vec2 direction) {
  vec3 color = vec3(0.0);
  vec2 texelSize = 1.0 / textureSize(image, 0);
  int samples = spread * 2;

  for (int i = -spread; i <= spread; i++) {
    float weight = 1.5 * (spread - abs(i)) / float(spread);

    color += weight * texture(image, uv + direction * i * texelSize).rgb;
  }

  return color / float(samples);
}