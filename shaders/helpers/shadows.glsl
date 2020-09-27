#include <helpers/sampling.glsl>

vec3 getLightMapTransform(vec3 surfacePosition, mat4 lightMatrix) {
  vec4 lightSpacePosition = lightMatrix * vec4(surfacePosition * vec3(1.0, 1.0, -1.0), 1.0);

  return (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;
}

float getShadowFactor(vec3 surfacePosition, mat4 lightMatrix, sampler2D lightMap, float bias, float maxSoftness) {
  vec3 transform = getLightMapTransform(surfacePosition, lightMatrix);

  if (transform.z > 1.0) {
    return 1.0;
  }

  vec2 texelSize = 1.0 / textureSize(lightMap, 0);
  vec2 sampleSpread = maxSoftness * texelSize * 0.5;
  float occludingSurfaceDistance = transform.z;

  for (int s = 0; s < 9; s++) {
    float sampleDistance = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_9[s] * sampleSpread).r;

    occludingSurfaceDistance = min(sampleDistance, occludingSurfaceDistance);
  }

  float shadowDistance = transform.z - occludingSurfaceDistance;
  float blur = maxSoftness * shadowDistance;
  float shadowFactor = 0.0;

  for (int i = 1; i <= 2; i++) {
    for (int s = 0; s < 9; s++) {
      float closestDepth = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_9[s] * float(i) * texelSize * blur * 0.5).r;

      shadowFactor += (closestDepth < transform.z - bias) ? 0.0 : 1.0;
    }
  }

  return shadowFactor / 18.0;
}