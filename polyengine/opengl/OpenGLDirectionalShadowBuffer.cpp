#include "opengl/OpenGLDirectionalShadowBuffer.h"
#include "opengl/FrameBuffer.h"

void OpenGLDirectionalShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE3);   // Shadow map cascade 0
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE4);   // Shadow map cascade 1
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE5);   // Shadow map cascade 2
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE6);   // Shadow map cascade 3
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}

void OpenGLDirectionalShadowBuffer::writeToShadowCascade(unsigned int cascadeIndex) {
  frameBuffer->bindColorTexture(GL_COLOR_ATTACHMENT0 + cascadeIndex);
}