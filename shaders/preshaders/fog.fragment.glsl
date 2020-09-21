#version 330 core

uniform sampler2D normalDepthTexture;
uniform sampler2D positionTexture;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepth;

void main() {
  vec3 position = texture(positionTexture, fragmentUv).xyz;
  float depth = texture(normalDepthTexture, fragmentUv).w;
  float a = clamp((depth / 10000.0) - (position.y / 1000.0), 0.0, 1.0);
  vec3 fogColor = mix(vec3(0.0), vec3(0.5), a);

  colorDepth = vec4(fogColor, depth);
}