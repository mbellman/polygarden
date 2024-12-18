const float GAUSSIAN_KERNEL_3[3] = float[](0.27901, 0.44198, 0.27901);
const float GAUSSIAN_KERNEL_5[5] = float[](0.06136, 0.24477, 0.38774, 0.24477, 0.06136);
const float GAUSSIAN_KERNEL_9[9] = float[](0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229);

vec4 gaussian3(sampler2D image, vec2 uv, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 texelSize = 1.0 / textureSize(image, 0);

  for (int i = 0; i < 3; i++) {
    vec2 offset = direction * texelSize * float(i - 1.0);
    float weight = GAUSSIAN_KERNEL_3[i];

    color += texture(image, uv + offset) * weight;
  }

  return color;
}

vec4 gaussian5(sampler2D image, vec2 uv, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 texelSize = 1.0 / textureSize(image, 0);

  for (int i = 0; i < 5; i++) {
    vec2 offset = direction * texelSize * float(i - 2.0);
    float weight = GAUSSIAN_KERNEL_5[i];

    color += texture(image, uv + offset) * weight;
  }

  return color;
}

vec4 gaussian9(sampler2D image, vec2 uv, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 texelSize = 1.0 / textureSize(image, 0);

  for (int i = 0; i < 9; i++) {
    vec2 offset = direction * texelSize * float(i - 4.0);
    float weight = GAUSSIAN_KERNEL_9[i];

    color += texture(image, uv + offset) * weight;
  }

  return color;
}