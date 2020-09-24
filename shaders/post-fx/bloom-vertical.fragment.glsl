#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 colorOut;

void main() {
  colorOut = getWeightedSpread(colorIn, fragmentUv, 10, vec2(0.0, 1.0));
}