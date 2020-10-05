#include "opengl/OpenGLDirectionalShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

OpenGLDirectionalShadowBuffer::OpenGLDirectionalShadowBuffer() {
  createShaderPrograms();
} 

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

void OpenGLDirectionalShadowBuffer::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/lightview.fragment.glsl"));
  lightViewProgram.link();

  cameraViewProgram.create();
  cameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  cameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/directional-shadowcaster.fragment.glsl"));
  cameraViewProgram.link();
}

ShaderProgram& OpenGLDirectionalShadowBuffer::getCameraViewProgram() {
  return cameraViewProgram;
}

ShaderProgram& OpenGLDirectionalShadowBuffer::getLightViewProgram() {
  return lightViewProgram;
}

void OpenGLDirectionalShadowBuffer::writeToShadowCascade(unsigned int cascadeIndex) {
  frameBuffer->bindColorTexture(GL_COLOR_ATTACHMENT0 + cascadeIndex);
}