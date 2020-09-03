#version 330 core

#include <polyengine/shaders/helpers/vertex-transformers.glsl>

uniform mat4 lightMatrix;

layout (location = 0) in vec3 vertexPosition;
layout (location = 5) in mat4 modelMatrix;

void main() {
  gl_Position = lightMatrix * modelMatrix * vec4(getTransformedVertex(vertexPosition), 1.0);
}