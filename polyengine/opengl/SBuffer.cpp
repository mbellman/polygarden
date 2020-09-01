#include "opengl/SBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

SBuffer::SBuffer() {
  createShaderPrograms();

  glScreenQuad = new OpenGLScreenQuad();
}

void SBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE3);    // Shadowcaster light view buffer, cascade 0
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE4);    // Shadowcaster light view buffer, cascade 1
  frameBuffer->addColorTexture(GL_R32F, GL_RED, GL_CLAMP_TO_BORDER, GL_TEXTURE5);    // Shadowcaster light view buffer, cascade 2
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}

void SBuffer::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/lightview.fragment.glsl"));
  lightViewProgram.link();
  lightViewProgram.use();

  directionalShadowProgram.create();
  directionalShadowProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/quad.vertex.glsl"));
  directionalShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/directional-shadowcaster.fragment.glsl"));
  directionalShadowProgram.link();
  directionalShadowProgram.use();

  spotShadowProgram.create();
  spotShadowProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/quad.vertex.glsl"));
  spotShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/spot-shadowcaster.fragment.glsl"));
  spotShadowProgram.link();
  spotShadowProgram.use();
}

ShaderProgram& SBuffer::getDirectionalShadowProgram() {
  return directionalShadowProgram;
}

ShaderProgram& SBuffer::getLightViewProgram() {
  return lightViewProgram;
}

ShaderProgram& SBuffer::getSpotShadowProgram() {
  return spotShadowProgram;
}

void SBuffer::writeToShadowCascade(unsigned int cascadeIndex) {
  frameBuffer->bindColorTexture(GL_COLOR_ATTACHMENT0 + cascadeIndex);
}