#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorDepthIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepthOut;

float getColorDelta(vec3 colorA, vec3 colorB) {
  return (
    abs(colorA.r - colorB.r) +
    abs(colorA.g - colorB.g) +
    abs(colorA.b - colorB.b)
  );
}

vec3 getMixed(vec3 color, vec2 texelSize) {
  for (int i = 0; i < 4; i++) {
    color += texture(colorDepthIn, fragmentUv + CROSS_SAMPLE_OFFSETS[i] * texelSize).rgb * 0.5;
  }

  for (int i = 0; i < 4; i++) {
    color += texture(colorDepthIn, fragmentUv + DIAMOND_SAMPLE_OFFSETS[i] * texelSize).rgb * 0.25;
  }

  return color / 4.0;
}

void main() {
  vec2 texelSize = 1.0 / textureSize(colorDepthIn, 0);
  vec4 colorDepth = texture(colorDepthIn, fragmentUv);
  vec3 color = colorDepth.rgb;

  for (int i = 0; i < 4; i++) {
    vec3 comparisonColor = texture(colorDepthIn, fragmentUv + CROSS_SAMPLE_OFFSETS[i] * texelSize).rgb;
    float delta = getColorDelta(color, comparisonColor);

    if (delta > 0.15) {
      color = getMixed(color, texelSize);
    }
  }

  colorDepthOut = vec4(color, colorDepth.w);
}