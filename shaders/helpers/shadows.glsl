#include <helpers/sampling.glsl>
#include <helpers/random.glsl>

vec3 getLightMapTransform(vec3 surfacePosition, mat4 lightMatrix) {
  vec4 lightSpacePosition = lightMatrix * vec4(surfacePosition * vec3(1.0, 1.0, -1.0), 1.0);

  return (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;
}

float getVsmShadowFactor(vec3 surfacePosition, mat4 lightMatrix, sampler2D lightMap) {
  vec3 transform = getLightMapTransform(surfacePosition, lightMatrix);
  vec2 moments = texture(lightMap, transform.xy).rg;

  if (transform.z > 1.0 || transform.z <= moments.x) {
    return 1.0;
  }

  float variance = max(moments.y - moments.x * moments.x, 0.000005);
  float delta = transform.z - moments.x;

  return variance / (variance + delta * delta);
}

float getPcfShadowFactor(vec3 surfacePosition, mat4 lightMatrix, sampler2D lightMap, float bias, float maxSoftness) {
  vec3 transform = getLightMapTransform(surfacePosition, lightMatrix);

  if (transform.z > 1.0) {
    return 1.0;
  }

  vec2 texelSize = 1.0 / textureSize(lightMap, 0);
  vec2 sampleSpread = maxSoftness * texelSize * 0.25;
  float closestDepth = texture(lightMap, transform.xy).r;
  float occludingSurfaceDistance = min(transform.z, closestDepth);
  float shadowFactor = 0.0;

  shadowFactor += ((closestDepth < transform.z - bias) ? 0.0 : 1.0);

  for (int s = 0; s < 8; s++) {
    float sampleDistance = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_8[s] * sampleSpread).r;

    occludingSurfaceDistance = min(sampleDistance, occludingSurfaceDistance);
  }

  float shadowDistance = transform.z - occludingSurfaceDistance;
  float blur = 1.0 + maxSoftness * shadowDistance;

  for (int i = 1; i <= 2; i++) {
    for (int s = 0; s < 8; s++) {
      vec2 radialOffset = RADIAL_SAMPLE_OFFSETS_8[s] * float(i) * texelSize * blur * 0.5;
      vec2 randomOffset = getRandomOffset(float(s * i)) * float(i) * texelSize * 0.5;
      float sampledClosestDepth = texture(lightMap, transform.xy + radialOffset + randomOffset).r;

      shadowFactor += ((sampledClosestDepth < transform.z - bias) ? 0.0 : 1.0);
    }
  }

  return shadowFactor / 17.0;
}