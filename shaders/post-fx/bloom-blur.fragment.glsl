#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 colorOut;

float weights[7] = float[](
  0.015625,
  0.093750,
  0.234375,
  0.312500,
  0.234375,
  0.093750,
  0.015625
);

void main() {
  vec3 color = texture(colorIn, fragmentUv).rgb;
  vec2 texelSize = 1.0 / textureSize(colorIn, 0);

  for (int y = -3; y <= 3; y++) {
    for (int x = -3; x <= 3; x++) {
      float weight = 2.0 * (weights[y + 3] + weights[x + 3]);

      color += weight * texture(colorIn, fragmentUv + vec2(x, y) * texelSize).rgb;
    }
  }

  colorOut = color / 50.0;
}