#version 330 core

#include <helpers/gaussian.glsl>

uniform sampler2D cascade0;
uniform sampler2D cascade1;
uniform sampler2D cascade2;
uniform sampler2D cascade3;
uniform vec2 direction;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec2 colorOut0;
layout (location = 1) out vec2 colorOut1;
layout (location = 2) out vec2 colorOut2;
layout (location = 3) out vec2 colorOut3;

void main() {
  colorOut0 = gaussian3(cascade0, fragmentUv, direction).rg;
  colorOut1 = gaussian3(cascade1, fragmentUv, direction).rg;
  colorOut2 = gaussian3(cascade2, fragmentUv, direction).rg;
  colorOut3 = gaussian3(cascade3, fragmentUv, direction).rg;
}