#include <helpers/sampling.glsl>
#include <helpers/random.glsl>

vec3 getLightMapTransform(vec3 surfacePosition, mat4 lightMatrix) {
  vec4 lightSpacePosition = lightMatrix * vec4(surfacePosition * vec3(1.0, 1.0, -1.0), 1.0);

  return (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;
}

float getShadowFactor(vec3 surfacePosition, mat4 lightMatrix, sampler2D lightMap, float bias, float maxSoftness) {
  vec3 transform = getLightMapTransform(surfacePosition, lightMatrix);

  if (transform.z > 1.0) {
    // Ignore surfaces beyond the far plane in light-space
    return 1.0;
  }

  vec2 texelSize = 1.0 / textureSize(lightMap, 0);
  vec2 sampleSpread = maxSoftness * texelSize * 0.25;
  float closestDepth = texture(lightMap, transform.xy).r;
  float closestNeighboringOccluderDepth = min(transform.z, closestDepth);
  bool isSurfaceOccluded = closestDepth < transform.z - bias;
  float shadowFactor = 0.0;

  shadowFactor += isSurfaceOccluded ? 0.0 : 1.0;

  // Do a prelimary radial sweep of the surface region to determine
  // the likely-closest depth of any neighboring occluders
  for (int s = 0; s < 8; s++) {
    float sampleDistance = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_8[s] * sampleSpread).r;

    closestNeighboringOccluderDepth = min(sampleDistance, closestNeighboringOccluderDepth);
  }

  // Use the distance between the light-space surface depth
  // and closest neighboring occluder depth to determine blur
  float occluderDistance = transform.z - closestNeighboringOccluderDepth;
  float blur = 1.0 + maxSoftness * occluderDistance;

  // Sample the region to take an average shadow factor
  for (int i = 1; i <= 2; i++) {
    for (int s = 0; s < 8; s++) {
      vec2 radialOffset = RADIAL_SAMPLE_OFFSETS_8[s] * float(i) * texelSize * blur * 0.5;
      vec2 randomOffset = getRandomOffset2(float(s * i)) * float(i) * texelSize * 0.3;
      float sampledClosestDepth = texture(lightMap, transform.xy + radialOffset + randomOffset).r;

      shadowFactor += (sampledClosestDepth < transform.z - bias) ? 0.0 : 1.0;
    }
  }

  shadowFactor /= 17.0;

  // Correct for occluded surfaces where random shadow sample
  // offsets erroneously result in low average occlusion
  return (isSurfaceOccluded && shadowFactor > 0.9) ? 0.0 : shadowFactor;
}