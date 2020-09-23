#include "opengl/post-fx/BloomShader.h"

BloomShader::~BloomShader() {
  delete blurLevel1;
  delete blurLevel2;
  delete glScreenQuad;
}

void BloomShader::onInit() {
  addShaderProgram("color-separation", "./shaders/post-fx/bloom-color-separation.fragment.glsl");
  addShaderProgram("blur", "./shaders/post-fx/bloom-blur.fragment.glsl");
  addShaderProgram("combine", "./shaders/post-fx/bloom-combine.fragment.glsl");

  blurLevel1 = new FrameBuffer(960, 540);
  blurLevel2 = new FrameBuffer(480, 270);
  glScreenQuad = new OpenGLScreenQuad();

  blurLevel1->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE1);
  blurLevel2->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);

  blurLevel1->bindColorTextures();
  blurLevel2->bindColorTextures();
}

void BloomShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void BloomShader::onRender() {
  frameBuffer->blit(blurLevel1);

  getShaderProgram("color-separation")->use();
  getShaderProgram("color-separation")->setInt("colorIn", 1);

  blurLevel1->startReading();
  blurLevel2->startWriting();
  glScreenQuad->render();

  getShaderProgram("blur")->use();
  getShaderProgram("blur")->setInt("colorIn", 0);

  blurLevel1->startWriting();
  blurLevel2->startReading();
  glScreenQuad->render();

  getShaderProgram("combine")->use();
  getShaderProgram("combine")->setInt("colorDepthIn", 0);
  getShaderProgram("combine")->setInt("bloomColorIn", 1);

  blurLevel1->startReading();
  frameBuffer->startReading();
  writeToOutputBuffer();
}