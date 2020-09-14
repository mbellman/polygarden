#version 330 core

#include <helpers/blur.glsl>

uniform sampler2D baseColor;
uniform sampler2D bloomColor;

in vec2 fragmentUv;

layout (location = 0) out vec3 color;

const vec2 SAMPLE_OFFSETS[8] = vec2[8](
  vec2(-0.7, -0.7),
  vec2(0.0, -1.0),
  vec2(0.7, -0.7),
  vec2(-1.0, 0.0),
  vec2(1.0, 0.0),
  vec2(-0.7, 0.7),
  vec2(0.0, 1.0),
  vec2(0.7, 0.7)
);

void main() {
  vec2 texelSize = 1.0 / textureSize(bloomColor, 0);
  vec3 base = texture(baseColor, fragmentUv).xyz;
  vec3 bloom = vec3(0.0);
  
  for (int i = 1; i <= 3; i++) {
    for (int s = 0; s < 8; s++) {
      float factor = 4.0 - float(i);
      vec2 sampleUv = fragmentUv + SAMPLE_OFFSETS[s] * texelSize * i * 2.5;

      bloom += factor * texture(bloomColor, sampleUv).xyz;
    }
  }

  color = base + bloom / 24.0;
}