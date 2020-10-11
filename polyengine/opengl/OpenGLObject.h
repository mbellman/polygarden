#pragma once

#include <map>
#include <string>
#include <vector>

#include "glew.h"
#include "glut.h"
#include "subsystem/entities/Object.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"

struct OpenGLObjectLod {
  GLuint vao;
  GLuint ebo;
  GLuint buffers[4];
  const Object* baseObject = nullptr;
};

class OpenGLObject {
public:
  OpenGLObject(Object* object);
  ~OpenGLObject();

  static void freeCachedResources();

  void bindTextures();
  Object* getSourceObject() const;
  bool hasNormalMap() const;
  bool hasTexture() const;
  void render();
  void renderLod(unsigned int index);
  void renderShadowLod();

private:
  static std::map<int, OpenGLTexture*> textureMap;
  static std::map<std::string, ShaderProgram*> shaderMap;

  std::vector<OpenGLObjectLod*> glLods;
  unsigned int activeLodIndex = 0;
  Object* sourceObject = nullptr;
  OpenGLTexture* glTexture = nullptr;
  OpenGLTexture* glNormalMap = nullptr;

  static OpenGLTexture* createOpenGLTexture(const Texture* texture, GLenum unit);

  void addLod(const Object* object);
  void bufferDynamicData(const void* data, unsigned int size, GLuint vbo);
  void bufferInstanceData();
  void bufferVertexData();
  void bufferVertexElementData();
  void defineColorAttributes();
  void defineMatrixAttributes();
  void defineObjectIdAttributes();
  void defineVertexAttributes();
  OpenGLObjectLod* getActiveLod();
  void setActiveLodIndex(unsigned int index);
};