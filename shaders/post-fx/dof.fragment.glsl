#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorDepthIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepthOut;

const float MAX_BLUR_DEPTH = 1000.0;

vec2 getBlur(float depth) {
  vec2 MIN_BLUR = vec2(0.0);
  vec2 MAX_BLUR = 1.0 / textureSize(colorDepthIn, 0);

  float r = depth / MAX_BLUR_DEPTH;

  if (r >= 1.0) {
    return MAX_BLUR;
  }

  return mix(MIN_BLUR, MAX_BLUR, pow(r, 3));
}

void main() {
  vec4 colorDepth = texture(colorDepthIn, fragmentUv);
  float depth = colorDepth.w;
  vec2 blur = getBlur(depth);
  vec3 color = colorDepth.rgb;

  for (int s = 0; s < 6; s++) {
    color += texture(colorDepthIn, fragmentUv + RADIAL_SAMPLE_OFFSETS_6[s] * blur * 1.7).rgb;
  }

  colorDepthOut = vec4(color / 7.0, depth);
}