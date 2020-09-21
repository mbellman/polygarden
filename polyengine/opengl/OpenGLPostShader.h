#pragma once

#include <functional>

#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"
#include "opengl/FrameBuffer.h"
#include "subsystem/Math.h"

typedef std::function<FrameBuffer*(const Region2d<int>&)> FrameBufferFactory;
typedef std::function<void(const ShaderProgram&)> RenderHandler;

class OpenGLPostShader {
public:
  OpenGLPostShader(const char* path);
  ~OpenGLPostShader();

  void createFrameBuffer(const Region2d<int>& screen);
  FrameBuffer* getFrameBuffer() const;
  void render();
  void setFrameBufferFactory(FrameBufferFactory factory);
  void setRenderHandler(RenderHandler handler);
  void startWriting();

private:
  ShaderProgram program;
  OpenGLScreenQuad* glScreenQuad = nullptr;
  FrameBuffer* frameBuffer = nullptr;
  FrameBufferFactory frameBufferFactory = nullptr;
  RenderHandler renderHandler = nullptr;
};