#version 330 core

#include <helpers/lighting.glsl>
#include <helpers/shadows.glsl>
#include <helpers/dithering.glsl>

uniform sampler2D colorTexture;
uniform sampler2D normalDepthTexture;
uniform sampler2D positionTexture;
uniform sampler2D lightMaps[3];
uniform mat4 lightMatrixCascades[3];
uniform vec3 cameraPosition;
uniform Light light;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepth;

int getCascadeIndex(vec3 position) {
  float fragDistance = length(cameraPosition - position);

  if (fragDistance < 150.0) {
    return 0;
  } else if (fragDistance < 500.0) {
    return 1;
  } else {
    return 2;
  }
}

float getBias(int cascadeIndex) {
  switch (cascadeIndex) {
    case 0:
    case 1:
      return 0.0001;
    case 2:
      return 0.0002;
  }
}

vec3 getVolumetricLight(vec3 surfacePosition) {
  const int SAMPLE_COUNT = 15;

  vec3 surfaceToCamera = cameraPosition - surfacePosition;
  float sampleFactor = 1.0 / float(SAMPLE_COUNT);
  vec3 volumetricLight = vec3(0.0);
  vec3 ray = surfaceToCamera * sampleFactor;

  surfacePosition += ray * getDithering(fragmentUv, textureSize(positionTexture, 0));

  for (int i = 1; i < SAMPLE_COUNT; i++) {
    vec3 samplePosition = surfacePosition + ray * float(i);
    int cascadeIndex = getCascadeIndex(samplePosition);
    mat4 lightMatrix = lightMatrixCascades[cascadeIndex];
    vec3 transform = getLightMapTransform(samplePosition, lightMatrix);
    float closestDepth = texture(lightMaps[cascadeIndex], transform.xy).r;

    volumetricLight += (closestDepth < transform.z) ? vec3(0.0) : (light.color * sampleFactor);
  }

  return volumetricLight * 0.25;
}

void main() {
  vec3 albedo = texture(colorTexture, fragmentUv).xyz;
  vec3 position = texture(positionTexture, fragmentUv).xyz;
  vec4 normalDepth = texture(normalDepthTexture, fragmentUv);
  vec3 surfaceToCamera = normalize(cameraPosition - position);
  vec3 normal = normalDepth.xyz;
  vec3 lighting = albedo * getDirectionalLightFactor(light, normal, surfaceToCamera);
  int cascadeIndex = getCascadeIndex(position);
  float shadowFactor = getShadowFactor(position, lightMatrixCascades[cascadeIndex], lightMaps[cascadeIndex], getBias(cascadeIndex));
  vec3 volumetricLight = getVolumetricLight(position);

  colorDepth = vec4(lighting * shadowFactor + volumetricLight, normalDepth.w);
}