#version 330 core

#include <polyengine/shaders/helpers/blur.glsl>

uniform sampler2D baseColor;
uniform sampler2D bloomColor;

in vec2 fragmentUv;

layout (location = 0) out vec3 color;

void main() {
  vec3 base = texture(baseColor, fragmentUv).xyz;
  vec3 bloom = getBlur(bloomColor, fragmentUv, 20, vec2(0.0, 1.0)) * 0.5;

  color = base + bloom;
}