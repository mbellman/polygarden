#include "opengl/OpenGLSpotShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

OpenGLSpotShadowBuffer::OpenGLSpotShadowBuffer() {
  createShaderPrograms();
} 

void OpenGLSpotShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE3);
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}

void OpenGLSpotShadowBuffer::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/lightview.fragment.glsl"));
  lightViewProgram.link();

  cameraViewProgram.create();
  cameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  cameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/spot-shadowcaster.fragment.glsl"));
  cameraViewProgram.link();
}

ShaderProgram& OpenGLSpotShadowBuffer::getCameraViewProgram() {
  return cameraViewProgram;
}

ShaderProgram& OpenGLSpotShadowBuffer::getLightViewProgram() {
  return lightViewProgram;
}