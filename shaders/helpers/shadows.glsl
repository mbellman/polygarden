const vec2 SAMPLE_OFFSETS[8] = vec2[8](
  vec2(-0.7, -0.7),
  vec2(0.0, -1.0),
  vec2(0.7, -0.7),
  vec2(-1.0, 0.0),
  vec2(1.0, 0.0),
  vec2(-0.7, 0.7),
  vec2(0.0, 1.0),
  vec2(0.7, 0.7)
);

const float MAX_SOFTNESS = 50.0;

float getShadowFactor(vec3 worldPosition, mat4 lightMatrix, sampler2D lightMap, float bias) {
  vec4 lightSpacePosition = lightMatrix * vec4(worldPosition * vec3(1.0, 1.0, -1.0), 1.0);
  vec3 projection = (lightSpacePosition.xyz / lightSpacePosition.w) * 0.5 + 0.5;
  vec2 texelSize = 0.5 * 1.0 / textureSize(lightMap, 0);
  vec2 sampleSpread = texelSize * MAX_SOFTNESS;
  float occludingSurfaceDistance = 1.0;

  for (int i = 0; i < 8; i++) {
    float sampleDistance = texture(lightMap, projection.xy + SAMPLE_OFFSETS[i] * sampleSpread).r;

    if (sampleDistance < occludingSurfaceDistance) {
      occludingSurfaceDistance = sampleDistance;
    }
  }

  float shadowDistance = projection.z - occludingSurfaceDistance;
  float blur = 1.0 + MAX_SOFTNESS * shadowDistance;
  float shadowSum = 0.0;

  for (int x = -2; x <= 2; x++) {
    for (int y = -2; y <= 2; y++) {
      float closestDepth = texture(lightMap, projection.xy + vec2(x, y) * texelSize * blur).r;

      shadowSum += (closestDepth < projection.z - bias) ? 0.0 : 1.0;
    }
  }

  return min(shadowSum / 25.0, 1.0);
}