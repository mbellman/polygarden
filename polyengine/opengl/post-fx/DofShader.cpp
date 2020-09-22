#include "opengl/post-fx/DofShader.h"
#include "opengl/OpenGLDebugger.h"

void DofShader::onInit() {
  addShaderProgram("./shaders/post-fx/dof.fragment.glsl");
}

void DofShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void DofShader::onRender() {
  getShaderProgram()->use();
}