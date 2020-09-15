#pragma once

#include <map>
#include <string>

#include "glew.h"
#include "glut.h"
#include "subsystem/entities/Object.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"

class OpenGLObject {
public:
  OpenGLObject(Object* object);
  ~OpenGLObject();

  static void freeCachedResources();

  void bindTextures();
  ShaderProgram* getCustomShader() const;
  Object* getSourceObject() const;
  bool hasNormalMap() const;
  bool hasCustomShader() const;
  bool hasTexture() const;
  void render();

private:
  static std::map<int, OpenGLTexture*> textureMap;
  static std::map<std::string, ShaderProgram*> shaderMap;

  GLuint vao;
  GLuint buffers[4];
  unsigned int previousTotalInstances = 0;
  Object* sourceObject = nullptr;
  OpenGLTexture* glTexture = nullptr;
  OpenGLTexture* glNormalMap = nullptr;
  ShaderProgram* customShader = nullptr;

  static OpenGLTexture* createOpenGLTexture(const Texture* texture, GLenum unit);
  static ShaderProgram* createShaderProgram(std::string fragShaderPath);

  void bufferColorData();
  void bufferInstanceData(const void* data, unsigned int size, GLuint vbo);
  void bufferObjectIdData();
  void bufferMatrixData();
  void bufferVertexData();
  void defineColorAttributes();
  void defineMatrixAttributes();
  void defineObjectIdAttributes();
  void defineVertexAttributes();
};