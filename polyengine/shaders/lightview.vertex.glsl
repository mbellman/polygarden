#version 330 core

#include <helpers/attributes.glsl>
#include <helpers/vertex-transformers.glsl>

uniform mat4 lightMatrix;

out vec2 fragmentUv;

void main() {
  gl_Position = lightMatrix * Instance.matrix * vec4(getTransformedVertex(Vertex.position), 1.0);
  fragmentUv = Vertex.uv;
}