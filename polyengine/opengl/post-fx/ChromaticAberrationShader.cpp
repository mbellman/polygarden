#include "opengl/post-fx/ChromaticAberrationShader.h"

void ChromaticAberrationShader::onInit() {
  addShaderProgram("./shaders/post-fx/chromatic-aberration.fragment.glsl");
}

void ChromaticAberrationShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void ChromaticAberrationShader::onRender() {
  getShaderProgram()->use();
}