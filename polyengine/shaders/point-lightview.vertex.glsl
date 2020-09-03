#version 330 core

#include <polyengine/shaders/helpers/vertex-transformers.glsl>

layout (location = 0) in vec3 vertexPosition;
layout (location = 5) in mat4 modelMatrix;

void main() {
  gl_Position = modelMatrix * vec4(getTransformedVertex(vertexPosition), 1.0);
}