#pragma once

#include <functional>
#include <map>
#include <string>

#include "opengl/ShaderProgram.h"
#include "opengl/FrameBuffer.h"
#include "subsystem/Math.h"

class AbstractOpenGLPostShader {
public:
  virtual ~AbstractOpenGLPostShader();

  void createFrameBuffer(const Area<unsigned int>& size);
  FrameBuffer* getFrameBuffer() const;
  void render();
  virtual void onInit() = 0;
  virtual void onConfigureFrameBuffer() = 0;
  virtual void onRender() = 0;
  void setNextShader(AbstractOpenGLPostShader* glPostShader);
  void writeToInputBuffer();
  void writeToOutputBuffer();

protected:
  FrameBuffer* frameBuffer = nullptr;

  void addShaderProgram(std::string name, const char* path);
  void addShaderProgram(const char* path);
  ShaderProgram* getShaderProgram(std::string name);
  ShaderProgram* getShaderProgram();

private:
  std::map<std::string, ShaderProgram*> programMap;
  AbstractOpenGLPostShader* nextShader = nullptr;
};