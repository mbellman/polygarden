#version 330 core

uniform bool hasTexture = false;
uniform sampler2D modelTexture;

in vec2 fragmentUv;

layout (location = 0) out float depth;

void main() {
  if (hasTexture && texture(modelTexture, fragmentUv).a == 0.0) {
    discard;
  }

  depth = gl_FragCoord.z;
}