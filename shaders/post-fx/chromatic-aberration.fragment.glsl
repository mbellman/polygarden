#version 330 core

uniform sampler2D screen;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 color;

vec3 getChromaticAberration() {
  vec2 clipUv = fragmentUv * 2.0 - 1.0;

  return vec3(
    texture(screen, clipUv * 0.999 * 0.5 + 0.5).r,
    texture(screen, clipUv * 0.995 * 0.5 + 0.5).g,
    texture(screen, clipUv * 0.99 * 0.5 + 0.5).b
  );
}

void main() {
  color = getChromaticAberration();
}