#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D screen;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 color;

const float MAX_BLUR_DEPTH = 1000.0;

vec2 getBlur(float depth) {
  vec2 MIN_BLUR = vec2(0.0);
  vec2 MAX_BLUR = 1.0 / textureSize(screen, 0);

  float r = depth / MAX_BLUR_DEPTH;

  if (r >= 1.0) {
    return MAX_BLUR;
  }

  return mix(MIN_BLUR, MAX_BLUR, pow(r, 3));
}

vec3 getDoF() {
  vec3 sum = vec3(0.0);
  float depth = texture(screen, fragmentUv).w;
  vec2 blur = getBlur(depth);

  for (int s = 0; s < 6; s++) {
    sum += texture(screen, fragmentUv + RADIAL_SAMPLE_OFFSETS_6[s] * blur * 1.5).xyz;
  }

  return sum / 6.0;
}

void main() {
  color = getDoF();
}