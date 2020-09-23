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
  blurLevel3 = new FrameBuffer(240, 135);
  glScreenQuad = new OpenGLScreenQuad();

  blurLevel1->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE1);
  blurLevel2->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);
  blurLevel3->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);

  blurLevel1->bindColorTextures();
  blurLevel2->bindColorTextures();
  blurLevel3->bindColorTextures();
}

void BloomShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void BloomShader::onRender() {
  getShaderProgram("color-separation")->use();
  getShaderProgram("color-separation")->setInt("colorDepthIn", 0);

  frameBuffer->startReading();
  blurLevel1->startWriting();
  glScreenQuad->render();

  blurLevel1->blit(blurLevel2);
  blurLevel2->blit(blurLevel3);

  getShaderProgram("blur")->use();
  getShaderProgram("blur")->setInt("colorIn", 0);

  blurLevel2->startWriting();
  blurLevel3->startReading();
  glScreenQuad->render();

  blurLevel2->blit(blurLevel1);

  getShaderProgram("combine")->use();
  getShaderProgram("combine")->setInt("colorDepthIn", 0);
  getShaderProgram("combine")->setInt("bloomColorIn", 1);

  blurLevel1->startReading();
  frameBuffer->startReading();
  writeToOutputBuffer();
}