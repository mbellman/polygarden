#include <helpers/sampling.glsl>

const float MAX_SOFTNESS = 30.0;

vec3 getLightMapTransform(vec3 worldPosition, mat4 lightMatrix) {
  vec4 lightSpacePosition = lightMatrix * vec4(worldPosition * vec3(1.0, 1.0, -1.0), 1.0);

  return (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;
}

float getShadowFactor(vec3 worldPosition, mat4 lightMatrix, sampler2D lightMap, float bias) {
  vec3 transform = getLightMapTransform(worldPosition, lightMatrix);
  vec2 texelSize = 1.0 / textureSize(lightMap, 0);
  vec2 sampleSpread = texelSize * MAX_SOFTNESS * 0.5;
  float occludingSurfaceDistance = transform.z;

  for (int i = 0; i < 9; i++) {
    float sampleDistance = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_9[i] * sampleSpread).r;

    if (sampleDistance < occludingSurfaceDistance) {
      occludingSurfaceDistance = sampleDistance;
    }
  }

  float shadowDistance = transform.z - occludingSurfaceDistance;
  float blur = 1.0 + MAX_SOFTNESS * shadowDistance;
  float shadowSum = 0.0;

  for (int x = 1; x <= 2; x++) {
    for (int s = 0; s < 9; s++) {
      float closestDepth = texture(lightMap, transform.xy + RADIAL_SAMPLE_OFFSETS_9[s] * float(x) * texelSize * blur).r;

      shadowSum += (closestDepth < transform.z - bias) ? 0.0 : 1.0;
    }
  }

  return shadowSum / 18.0;
}