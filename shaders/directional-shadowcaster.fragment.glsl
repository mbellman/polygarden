#version 330 core

#include <helpers/lighting.glsl>
#include <helpers/shadows.glsl>
#include <helpers/sampling.glsl>

uniform sampler2D colorTexture;
uniform sampler2D normalDepthTexture;
uniform sampler2D positionTexture;
uniform sampler2D lightMaps[4];
uniform mat4 lightMatrixCascades[4];
uniform vec3 cameraPosition;
uniform Light light;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepth;

int getCascadeIndex(float depth) {
  if (depth < 200.0) {
    return 0;
  } else if (depth < 500.0) {
    return 1;
  } else if (depth < 1250.0) {
    return 2;
  } else {
    return 3;
  }
}

float easeOut(float t) {
  return 1.0 - pow(1.0 - t, 50);
}

float getBias(float depth, vec3 normal) {
  float angle = 1.0 - max(dot(normal, light.direction), 0.0);
  float base = angle * 0.0001;

  return base + mix(0.00001, 0.001, easeOut(depth / 10000.0));
}

float getMaxSoftness(float depth) {
  return mix(30.0, 2.0, easeOut(depth / 10000.0));
}

vec3 getVolumetricLight(vec3 surfacePosition) {
  const int STEP_COUNT = 15;

  vec3 surfaceToCamera = cameraPosition - surfacePosition;
  float stepFactor = 1.0 / float(STEP_COUNT);
  vec3 volumetricLight = vec3(0.0);
  vec3 ray = surfaceToCamera * stepFactor;
  float strength = 0.2 + pow(max(dot(normalize(surfaceToCamera), light.direction), 0.0), 10);

  surfacePosition += ray * getDitheringFactor(fragmentUv, textureSize(positionTexture, 0));

  for (int i = 1; i < STEP_COUNT; i++) {
    vec3 samplePosition = surfacePosition + ray * float(i);
    float depth = length(samplePosition - cameraPosition);
    int cascadeIndex = getCascadeIndex(depth);
    mat4 lightMatrix = lightMatrixCascades[cascadeIndex];
    vec3 transform = getLightMapTransform(samplePosition, lightMatrix);
    float closestDepth = texture(lightMaps[cascadeIndex], transform.xy).r;

    volumetricLight += (closestDepth < transform.z) ? vec3(0.0) : (light.color * stepFactor);
  }

  return volumetricLight * strength;
}

void main() {
  vec3 albedo = texture(colorTexture, fragmentUv).xyz;
  vec3 position = texture(positionTexture, fragmentUv).xyz;
  vec4 normalDepth = texture(normalDepthTexture, fragmentUv);
  vec3 surfaceToCamera = normalize(cameraPosition - position);
  vec3 normal = normalDepth.xyz;
  float depth = normalDepth.w;

  int cascadeIndex = getCascadeIndex(depth);
  mat4 lightMatrix = lightMatrixCascades[cascadeIndex];
  vec3 lighting = albedo * getDirectionalLightFactor(light, normal, surfaceToCamera);
  float bias = getBias(depth, normal);
  float maxSoftness = getMaxSoftness(depth);
  float shadowFactor = getShadowFactor(position, lightMatrix, lightMaps[cascadeIndex], bias, maxSoftness);
  vec3 volumetricLight = getVolumetricLight(position);

  colorDepth = vec4(lighting * shadowFactor + volumetricLight, depth);
}