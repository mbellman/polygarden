#version 330 core

#define MAX_LIGHTS 128

#include <helpers/lighting.glsl>

const int POINT_LIGHT = 0;
const int DIRECTIONAL_LIGHT = 1;
const int SPOT_LIGHT = 2;

uniform sampler2D colorTexture;
uniform sampler2D normalDepthTexture;
uniform sampler2D positionTexture;
uniform vec3 cameraPosition;

noperspective in vec2 fragmentUv;
flat in Light light;

layout (location = 0) out vec4 colorDepth;

void main() {
  vec3 albedo = texture(colorTexture, fragmentUv).xyz;
  vec3 position = texture(positionTexture, fragmentUv).xyz;
  vec4 normalDepth = texture(normalDepthTexture, fragmentUv);
  vec3 surfaceToCamera = normalize(cameraPosition - position);
  vec3 normal = normalDepth.xyz;
  vec3 illuminatedColor = vec3(0.0);

  switch (light.type) {
    case POINT_LIGHT:
      illuminatedColor = albedo * getPointLightFactor(light, position, normal, surfaceToCamera);
      break;
    case DIRECTIONAL_LIGHT:
      illuminatedColor = albedo * getDirectionalLightFactor(light, normal, surfaceToCamera);
      break;
    case SPOT_LIGHT:
      illuminatedColor = albedo * getSpotLightFactor(light, position, normal, surfaceToCamera);
      break;
  }

  colorDepth = vec4(illuminatedColor, normalDepth.w);
}