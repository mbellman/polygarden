#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 color;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexUv;
layout (location = 4) in mat4 modelMatrix;

out vec3 fragmentColor;
out vec3 fragmentNormal;
out vec3 fragmentTangent;
out vec3 fragmentPosition;
out vec2 fragmentUv;

vec4 getClipPosition() {
  return projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}

vec3 getWorldPosition() {
  vec3 position = vec4(modelMatrix * vec4(vertexPosition, 1.0)).xyz;

  return vec3(position.x, position.y, -position.z);
}

vec3 getNormal() {
  mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
  vec3 normal = normalMatrix * vertexNormal;

  return vec3(normal.x, normal.y, -normal.z);
}

vec3 getTangent() {
  mat3 tangentMatrix = transpose(inverse(mat3(modelMatrix)));
  vec3 tangent = tangentMatrix * vertexTangent;

  return vec3(tangent.x, tangent.y, -tangent.z);
}

vec2 getUv() {
  return vertexUv;
}

void main() {
  gl_Position = getClipPosition();

  fragmentColor = color;
  fragmentNormal = getNormal();
  fragmentTangent = getTangent();
  fragmentPosition = getWorldPosition();
  fragmentUv = getUv();
}