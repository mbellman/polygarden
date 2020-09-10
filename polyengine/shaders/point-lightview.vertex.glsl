#version 330 core

#include <helpers/attributes.glsl>
#include <helpers/vertex-transformers.glsl>

void main() {
  gl_Position = Instance.matrix * vec4(getTransformedVertex(Vertex.position), 1.0);
}