vec3 getBlur(sampler2D map, vec2 uv, int samples, vec2 direction) {
  vec4 sum = vec4(0.0);
  vec2 texelSize = 1.0 / textureSize(map, 0);
  int halfSamples = samples / 2;

  for (int i = -halfSamples; i <= halfSamples; i += 2) {
    sum += texture(map, uv + direction * i * texelSize);
  }

  return vec3(sum / float(halfSamples));
}