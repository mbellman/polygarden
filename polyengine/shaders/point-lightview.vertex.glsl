#version 330 core

in vec3 vertexPosition;
in mat4 modelMatrix;

void main() {
  gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}