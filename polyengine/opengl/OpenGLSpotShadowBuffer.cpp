#include "opengl/OpenGLSpotShadowBuffer.h"
#include "opengl/FrameBuffer.h"

void OpenGLSpotShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE3);
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}
