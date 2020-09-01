#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 5) in mat4 modelMatrix;

void main() {
  gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}