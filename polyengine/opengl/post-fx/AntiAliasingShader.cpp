#include "opengl/post-fx/AntiAliasingShader.h"

void AntiAliasingShader::onInit() {
  addShaderProgram("./shaders/post-fx/anti-aliasing.fragment.glsl");
}

void AntiAliasingShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void AntiAliasingShader::onRender() {
  getShaderProgram()->use();
}