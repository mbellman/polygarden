#version 330 core

uniform mat4 lightMatrix;

layout (location = 0) in vec3 vertexPosition;
layout (location = 4) in mat4 modelMatrix;

void main() {
  gl_Position = lightMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}