#pragma once

#include <map>
#include <string>

#include "glew.h"
#include "glut.h"
#include "subsystem/Entities.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"

class OpenGLObject {
public:
  OpenGLObject(const Object* object);
  ~OpenGLObject();

  static void freeCachedResources();

  void bindTextures();
  ShaderProgram* getCustomShader() const;
  const Object* getSourceObject() const;
  bool hasNormalMap() const;
  bool hasCustomShader() const;
  bool hasTexture() const;
  void render();

private:
  static std::map<int, OpenGLTexture*> textureMap;
  static std::map<std::string, ShaderProgram*> shaderMap;

  GLuint vao;
  GLuint buffers[3];
  unsigned int previousTotalInstances = 0;
  const Object* sourceObject = nullptr;
  OpenGLTexture* glTexture = nullptr;
  OpenGLTexture* glNormalMap = nullptr;
  ShaderProgram* customShader = nullptr;

  static OpenGLTexture* createOpenGLTexture(const Texture* texture, GLenum unit);
  static ShaderProgram* createShaderProgram(std::string fragShaderPath);

  void bufferColorData();
  void bufferInstanceData(const float* data, unsigned int size, GLuint vbo);
  void bufferMatrixData();
  void bufferVertexData();
  void defineColorAttributes();
  void defineMatrixAttributes();
  void defineVertexAttributes();
};