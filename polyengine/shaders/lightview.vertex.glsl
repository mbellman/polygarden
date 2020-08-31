#version 330 core

uniform mat4 lightMatrix;

in vec3 vertexPosition;
in mat4 modelMatrix;

void main() {
  gl_Position = lightMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}