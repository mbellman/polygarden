#include "opengl/post-fx/BloomShader.h"

BloomShader::~BloomShader() {
  delete downsample1;
  delete downsample2;
  delete downsample3_h;
  delete downsample3_v;
  delete glScreenQuad;
}

void BloomShader::onInit() {
  addShaderProgram("color-separation", "./shaders/post-fx/bloom-color-separation.fragment.glsl");
  addShaderProgram("horizontal", "./shaders/post-fx/bloom-horizontal.fragment.glsl");
  addShaderProgram("vertical", "./shaders/post-fx/bloom-vertical.fragment.glsl");
  addShaderProgram("combine", "./shaders/post-fx/bloom-combine.fragment.glsl");

  downsample1 = new FrameBuffer(960, 540);
  downsample2 = new FrameBuffer(480, 270);
  downsample3_h = new FrameBuffer(240, 135);
  downsample3_v = new FrameBuffer(240, 135);
  glScreenQuad = new OpenGLScreenQuad();

  downsample1->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE1);
  downsample2->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);
  downsample3_h->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);
  downsample3_v->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_EDGE, GL_TEXTURE0);

  downsample1->bindColorTextures();
  downsample2->bindColorTextures();
  downsample3_h->bindColorTextures();
  downsample3_v->bindColorTextures();
}

void BloomShader::onConfigureFrameBuffer() {
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE);
}

void BloomShader::onRender() {
  getShaderProgram("color-separation")->use();
  getShaderProgram("color-separation")->setInt("colorDepthIn", 0);

  frameBuffer->startReading();
  downsample1->startWriting();
  glScreenQuad->render();

  downsample1->blit(downsample2);
  downsample2->blit(downsample3_h);

  getShaderProgram("horizontal")->use();
  getShaderProgram("horizontal")->setInt("colorIn", 0);

  downsample3_h->startReading();
  downsample3_v->startWriting();
  glScreenQuad->render();

  getShaderProgram("vertical")->use();
  getShaderProgram("vertical")->setInt("colorIn", 0);

  downsample3_v->startReading();
  downsample2->startWriting();
  glScreenQuad->render();

  downsample2->blit(downsample1);

  getShaderProgram("combine")->use();
  getShaderProgram("combine")->setInt("colorDepthIn", 0);
  getShaderProgram("combine")->setInt("bloomColorIn", 1);

  downsample1->startReading();
  frameBuffer->startReading();
  writeToOutputBuffer();
}