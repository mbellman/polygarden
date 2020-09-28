#version 330 core

uniform bool hasTexture = false;
uniform sampler2D modelTexture;

in vec2 fragmentUv;

layout (location = 0) out vec2 depths;

void main() {
  if (hasTexture && texture(modelTexture, fragmentUv).a == 0.0) {
    discard;
  }

  depths = vec2(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z);
}