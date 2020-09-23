#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorDepthIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepthOut;

vec2 getBlur(float depth) {
  vec2 MIN_BLUR = vec2(0.0);
  vec2 MAX_BLUR = 1.0 / textureSize(colorDepthIn, 0);

  float focalDistance = min(texture(colorDepthIn, vec2(0.5, 0.5)).w, 500.0);
  float maxBlurDepth = min(focalDistance + 1000.0, 1500.0);
  float factor = depth < focalDistance ? 5.0 : 1.0;
  float alpha = factor * abs(depth - focalDistance) / maxBlurDepth;

  if (alpha >= 1.0) {
    return MAX_BLUR;
  }

  return mix(MIN_BLUR, MAX_BLUR, pow(alpha, 3));
}

void main() {
  vec4 colorDepth = texture(colorDepthIn, fragmentUv);
  float depth = colorDepth.w;
  vec2 blur = getBlur(depth);
  vec3 color = colorDepth.rgb;

  for (int s = 0; s < 4; s++) {
    color += texture(colorDepthIn, fragmentUv + CROSS_SAMPLE_OFFSETS[s] * blur * 1.5).rgb;
  }

  for (int s = 0; s < 4; s++) {
    color += texture(colorDepthIn, fragmentUv + DIAMOND_SAMPLE_OFFSETS[s] * blur * 3.0).rgb;
  }

  colorDepthOut = vec4(color / 9.0, depth);
}