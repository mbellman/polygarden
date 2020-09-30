#version 330 core

#include <helpers/sampling.glsl>

uniform sampler2D colorIn;
uniform vec2 direction;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 colorOut;

void main() {
  colorOut = getWeightedSpread(colorIn, fragmentUv, 10, direction);
}