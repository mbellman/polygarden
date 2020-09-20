#version 330 core

#include <helpers/lighting.glsl>

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexUv;
layout (location = 2) in vec2 quadOffset;
layout (location = 3) in vec2 quadScale;
layout (location = 4) in Light quadLight;

noperspective out vec2 fragmentUv;
flat out Light light;

void main() {
  gl_Position = vec4(vertexPosition * quadScale + quadOffset, 0.0, 1.0);

  fragmentUv = (2.0 * (vertexUv - 0.5) * quadScale + quadOffset) * 0.5 + 0.5;
  light = quadLight;
}