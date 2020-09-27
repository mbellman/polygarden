#version 330 core

#include <helpers/lighting.glsl>
#include <helpers/shadows.glsl>

uniform sampler2D colorTexture;
uniform sampler2D normalDepthTexture;
uniform sampler2D positionTexture;
uniform sampler2D lightMap;
uniform mat4 lightMatrix;
uniform vec3 cameraPosition;
uniform Light light;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepth;

float BIAS = 0.0001;
float MAX_SOFTNESS = 30.0;

void main() {
  vec3 albedo = texture(colorTexture, fragmentUv).xyz;
  vec3 position = texture(positionTexture, fragmentUv).xyz;
  vec4 normalDepth = texture(normalDepthTexture, fragmentUv);
  vec3 surfaceToCamera = normalize(cameraPosition - position);
  vec3 normal = normalDepth.xyz;
  vec3 lighting = albedo * getSpotLightFactor(light, position, normal, surfaceToCamera);
  float bias = 0.0001;
  float maxSoftness = 30.0;
  float shadowFactor = getShadowFactor(position, lightMatrix, lightMap, BIAS, MAX_SOFTNESS);

  colorDepth = vec4(lighting * shadowFactor, normalDepth.w);
}