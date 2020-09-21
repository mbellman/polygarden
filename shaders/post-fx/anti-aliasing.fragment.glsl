#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D screen;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 color;

float getColorDelta(vec3 colorA, vec3 colorB) {
  return (
    abs(colorA.r - colorB.r) +
    abs(colorA.g - colorB.g) +
    abs(colorA.b - colorB.b)
  );
}

vec3 getMixed(vec3 color, vec2 texelSize) {
  for (int i = 0; i < 8; i++) {
    color += texture(screen, fragmentUv + RADIAL_SAMPLE_OFFSETS_8[i] * texelSize).xyz * 0.25;
  }

  return color / 3.0;
}

vec3 getAntiAliasedColor() {
  vec2 texelSize = 1.0 / textureSize(screen, 0);
  vec3 color = texture(screen, fragmentUv).xyz;

  for (int i = 0; i < 4; i++) {
    vec3 comparison = texture(screen, fragmentUv + CROSS_SAMPLE_OFFSETS[i] * texelSize).xyz;
    float delta = getColorDelta(color, comparison);

    if (delta > 0.15) {
      color = getMixed(color, texelSize);
    }
  }

  return color;
}

void main() {
  color = getAntiAliasedColor();
}