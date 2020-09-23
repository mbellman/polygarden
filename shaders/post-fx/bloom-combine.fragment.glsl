#version 330 core

uniform sampler2D colorDepthIn;
uniform sampler2D bloomColorIn;

noperspective in vec2 fragmentUv;

layout (location = 0) out vec4 colorDepthOut;

void main() {
  colorDepthOut = texture(colorDepthIn, fragmentUv) + texture(bloomColorIn, fragmentUv);
}