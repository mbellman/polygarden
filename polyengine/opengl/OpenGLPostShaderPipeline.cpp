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

  if (glPostShaders.size() > 0) {
    glPostShaders[glPostShaders.size() - 1]->setNextShader(glPostShader);
  }

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

  for (auto* glPostShader : glPostShaders) {
    glPostShader->writeToOutputBuffer();
    glPostShader->render();
    glScreenQuad->render();
  }
}