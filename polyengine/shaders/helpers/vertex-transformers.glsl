#include <polyengine/shaders/helpers/time.glsl>

uniform int treeTransformFactor;
uniform int grassTransformFactor;

const float SPEED = 2.0;
const float DEVIATION = 0.1;

vec3 grass(vec3 position) {
  float offset = sin(time * SPEED + gl_InstanceID + position.y * 0.25) * DEVIATION * position.y * grassTransformFactor;

  return vec3(
    position.x + offset,
    position.y,
    position.z
  );
}

vec3 tree(vec3 position) {
  float magnitude = sqrt(position.x * position.x + position.z * position.z);
  float offset = sin(time * SPEED + gl_InstanceID) * DEVIATION * magnitude * min(position.y, 1.0) * treeTransformFactor;

  return vec3(
    position.x,
    position.y + offset,
    position.z
  );
}

vec3 getTransformedVertex(vec3 position) {
  return grass(tree(position));
}