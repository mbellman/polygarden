#pragma once

#include <vector>

#include "glew.h"
#include "glut.h"
#include "subsystem/Math.h"

struct ColorTexture {
  unsigned int internalFormat;
  unsigned int format;
  GLuint id;
  GLuint attachment;
  GLenum unit;
};

class FrameBuffer {
public:
  FrameBuffer(int width, int height);
  ~FrameBuffer();

  void addColorTexture(GLint internalFormat, GLenum format);
  void addColorTexture(GLint internalFormat, GLenum format, GLint clamp);
  void addColorTexture(GLint internalFormat, GLenum format, GLint clamp, GLenum unit);
  void addDepthCubeMap(GLenum unit);
  void addDepthStencilBuffer();
  void bindColorTexture(GLenum attachment);
  void bindColorTextures();
  void blit(FrameBuffer* target);
  void clearColorTexture(GLint attachment);
  void shareDepthStencilBuffer(FrameBuffer* target);
  void startReading();
  void startWriting();

private:
  GLuint fbo = 0;
  GLuint depthStencilBuffer = 0;
  GLuint depthCubeMap = 0;
  GLenum depthCubeMapUnit;
  std::vector<ColorTexture> colorTextures;
  Region2d<int> size;
};