const float DITHER_MATRIX[16] = float[16](
  0.0, 0.5, 0.125, 0.625,
  0.75, 0.22, 0.875, 0.375,
  0.1875, 0.6875, 0.0625, 0.5625,
  0.9375, 0.4375, 0.8125, 0.3125
);

float getDithering(vec2 fragment, vec2 screenSize) {
  int yOffset = 4 * (int(fragment.y * screenSize.y) % 4);
  int xOffset = int(fragment.x * screenSize.x) % 4;

  return DITHER_MATRIX[yOffset + xOffset];
}