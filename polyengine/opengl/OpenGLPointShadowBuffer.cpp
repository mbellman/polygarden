#include "opengl/OpenGLPointShadowBuffer.h"
#include "opengl/FrameBuffer.h"

void OpenGLPointShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addDepthCubeMap(GL_TEXTURE3);
}
