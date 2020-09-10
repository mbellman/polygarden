#version 330 core

#include <helpers/attributes.glsl>
#include <helpers/vertex-transformers.glsl>

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragmentColor;
out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentPosition;
out vec2 fragmentUv;

vec4 getClipPosition() {
  return projectionMatrix * viewMatrix * Instance.matrix * vec4(getTransformedVertex(Vertex.position), 1.0);
}

vec3 getWorldPosition() {
  vec3 position = vec4(Instance.matrix * vec4(getTransformedVertex(Vertex.position), 1.0)).xyz;

  return vec3(position.x, position.y, -position.z);
}

vec3 getNormal() {
  mat3 matrix = transpose(inverse(mat3(Instance.matrix)));
  vec3 normal = matrix * Vertex.normal;

  return vec3(normal.x, normal.y, -normal.z);
}

vec3 getTangent() {
  mat3 matrix = transpose(inverse(mat3(Instance.matrix)));
  vec3 tangent = matrix * Vertex.tangent;

  return vec3(tangent.x, tangent.y, -tangent.z);
}

void main() {
  gl_Position = getClipPosition();

  fragmentColor = Instance.color;
  fragmentNormal = getNormal();
  fragmentTangent = getTangent();
  fragmentPosition = getWorldPosition();
  fragmentUv = Vertex.uv;
}