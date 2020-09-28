#include "opengl/ShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

ShadowBuffer::ShadowBuffer() {
  createShaderPrograms();
}

void ShadowBuffer::blur() {
  frameBuffer->blit(downsample_x);

  blurProgram.use();
  blurProgram.setInt("cascade0", 0);
  blurProgram.setInt("cascade1", 1);
  blurProgram.setInt("cascade2", 2);
  blurProgram.setInt("cascade3", 3);
  blurProgram.setVec2f("direction", Vec2f(1.0f, 0.0f));

  downsample_x->startReading();
  downsample_y->startWriting();
  downsample_y->bindColorTextures();
  OpenGLScreenQuad::draw();

  blurProgram.setVec2f("direction", Vec2f(0.0f, 1.0f));

  downsample_y->startReading();
  downsample_x->startWriting();
  downsample_x->bindColorTextures();
  OpenGLScreenQuad::draw();

  downsample_x->blit(frameBuffer);
}

void ShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  if (downsample_x != nullptr) {
    delete downsample_x;
  }

  if (downsample_y != nullptr) {
    delete downsample_y;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_RG32F, GL_RG, GL_CLAMP_TO_BORDER, GL_TEXTURE3);    // Shadowcaster light view buffer, cascade 0
  frameBuffer->addColorTexture(GL_RG32F, GL_RG, GL_CLAMP_TO_BORDER, GL_TEXTURE4);    // Shadowcaster light view buffer, cascade 1
  frameBuffer->addColorTexture(GL_RG32F, GL_RG, GL_CLAMP_TO_BORDER, GL_TEXTURE5);    // Shadowcaster light view buffer, cascade 2
  frameBuffer->addColorTexture(GL_RG32F, GL_RG, GL_CLAMP_TO_BORDER, GL_TEXTURE6);    // Shadowcaster light view buffer, cascade 3
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();

  downsample_x = new FrameBuffer(width / 2, height / 2);

  downsample_x->addColorTexture(GL_RG32F, GL_RG);
  downsample_x->addColorTexture(GL_RG32F, GL_RG);
  downsample_x->addColorTexture(GL_RG32F, GL_RG);
  downsample_x->addColorTexture(GL_RG32F, GL_RG);
  downsample_x->bindColorTextures();

  downsample_y = new FrameBuffer(width / 2, height / 2);

  downsample_y->addColorTexture(GL_RG32F, GL_RG);
  downsample_y->addColorTexture(GL_RG32F, GL_RG);
  downsample_y->addColorTexture(GL_RG32F, GL_RG);
  downsample_y->addColorTexture(GL_RG32F, GL_RG);
  downsample_y->bindColorTextures();
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

  blurProgram.create();
  blurProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  blurProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/shadow-blur.fragment.glsl"));
  blurProgram.link();
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