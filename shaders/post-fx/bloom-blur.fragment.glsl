#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 colorOut;

void main() {
  vec3 color = texture(colorIn, fragmentUv).rgb;
  vec2 texelSize = 1.0 / textureSize(colorIn, 0);

  for (int y = -3; y <= 3; y++) {
    for (int x = -3; x <= 3; x++) {
      color += texture(colorIn, fragmentUv + vec2(x, y) * texelSize * 1.25).rgb;
    }
  }

  colorOut = color / 50.0;
}