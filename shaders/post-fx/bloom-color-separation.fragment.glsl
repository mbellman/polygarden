#version 330 core

uniform sampler2D colorIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec3 bloomColorOut;

bool isBloomColor(vec3 color) {
  return color.r > 0.9 || color.g > 0.9 || color.b > 0.9;
}

void main() {
  vec3 color = texture(colorIn, fragmentUv).rgb;

  bloomColorOut = isBloomColor(color) ? color : vec3(0.0);
}