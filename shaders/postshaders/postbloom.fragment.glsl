#version 330 core

#include <helpers/blur.glsl>
#include <helpers/sampling.glsl>

uniform sampler2D baseColor;
uniform sampler2D bloomColor;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 color;

void main() {
  vec2 screenSize = textureSize(bloomColor, 0);
  vec2 texelSize = 1.0 / screenSize;
  vec3 base = texture(baseColor, fragmentUv).xyz;
  vec3 bloom = vec3(0.0);
  
  for (int i = 1; i <= 3; i++) {
    for (int s = 0; s < 8; s++) {
      float factor = 4.0 - float(i);
      vec2 sampleUv = fragmentUv + RADIAL_SAMPLE_OFFSETS_8[s] * texelSize * i * 3.0;

      bloom += factor * texture(bloomColor, sampleUv).xyz;
    }
  }

  color = base + bloom / 24.0;
}