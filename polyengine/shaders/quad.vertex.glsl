#version 330 core

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 vertexUv;

out vec2 fragmentUv;

void main() {
  gl_Position = vec4(vertexPosition, 0.0, 1.0);

  fragmentUv = vertexUv;
}