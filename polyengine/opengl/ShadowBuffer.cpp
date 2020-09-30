#include "opengl/ShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

ShadowBuffer::ShadowBuffer() {
  createShaderPrograms();
} 

void ShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE3);   // Shadowcaster light view buffer, cascade 0
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE4);   // Shadowcaster light view buffer, cascade 1
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE5);   // Shadowcaster light view buffer, cascade 2
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE6);   // Shadowcaster light view buffer, cascade 3
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}

void ShadowBuffer::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/lightview.fragment.glsl"));
  lightViewProgram.link();

  directionalShadowProgram.create();
  directionalShadowProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  directionalShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/directional-shadowcaster.fragment.glsl"));
  directionalShadowProgram.link();

  spotShadowProgram.create();
  spotShadowProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  spotShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/spot-shadowcaster.fragment.glsl"));
  spotShadowProgram.link();
}

ShaderProgram& ShadowBuffer::getDirectionalShadowProgram() {
  return directionalShadowProgram;
}

ShaderProgram& ShadowBuffer::getLightViewProgram() {
  return lightViewProgram;
}

ShaderProgram& ShadowBuffer::getSpotShadowProgram() {
  return spotShadowProgram;
}

void ShadowBuffer::writeToShadowCascade(unsigned int cascadeIndex) {
  frameBuffer->bindColorTexture(GL_COLOR_ATTACHMENT0 + cascadeIndex);
}