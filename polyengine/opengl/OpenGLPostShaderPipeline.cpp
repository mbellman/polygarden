#include "opengl/OpenGLPostShaderPipeline.h"
#include "glew.h"
#include "glut.h"

OpenGLPostShaderPipeline::OpenGLPostShaderPipeline() {
  glScreenQuad = new OpenGLScreenQuad();
}

OpenGLPostShaderPipeline::~OpenGLPostShaderPipeline() {
  for (auto* glPostShader : glPostShaders) {
    delete glPostShader;
  }

  glPostShaders.clear();

  delete glScreenQuad;
}

void OpenGLPostShaderPipeline::addPostShader(AbstractOpenGLPostShader* glPostShader) {
  glPostShader->onInit();

  glPostShaders.push_back(glPostShader);
}

AbstractOpenGLPostShader* OpenGLPostShaderPipeline::getFirstShader() {
  return glPostShaders[0];
}

void OpenGLPostShaderPipeline::createFrameBuffers(const Region2d<int>& screen) {
  for (auto* glPostShader : glPostShaders) {
    glPostShader->createFrameBuffer(screen);
  }
}

void OpenGLPostShaderPipeline::render() {
  glDisable(GL_STENCIL_TEST);

  for (int i = 0; i < glPostShaders.size(); i++) {
    bool isFinalShader = i == glPostShaders.size() - 1;

    if (isFinalShader) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    } else {
      glPostShaders[i + 1]->startWriting();
    }

    glPostShaders[i]->render();
    glScreenQuad->render();
  }
}