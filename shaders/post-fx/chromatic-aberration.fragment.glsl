#version 330 core

uniform sampler2D colorDepthIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepthOut;

void main() {
  float depth = texture(colorDepthIn, fragmentUv).w;
  vec2 clipUv = fragmentUv * 2.0 - 1.0;

  colorDepthOut = vec4(
    texture(colorDepthIn, clipUv * 0.999 * 0.5 + 0.5).r,
    texture(colorDepthIn, clipUv * 0.995 * 0.5 + 0.5).g,
    texture(colorDepthIn, clipUv * 0.99 * 0.5 + 0.5).b,
    depth
  );
}