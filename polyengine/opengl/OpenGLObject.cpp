#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"

const static enum Buffer {
  VERTEX,
  MATRIX,
  COLOR,
  ID
};

const static enum Attribute {
  VERTEX_POSITION = 0,
  VERTEX_NORMAL = 1,
  VERTEX_TANGENT = 2,
  VERTEX_UV = 3,
  OBJECT_ID = 4,
  MODEL_COLOR = 5,
  MODEL_MATRIX = 6
};

OpenGLObject::OpenGLObject(Object* object) {
  sourceObject = object;

  glGenVertexArrays(1, &vao);
  glGenBuffers(4, &buffers[0]);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  bufferVertexData();
  bufferVertexElementData();

  defineVertexAttributes();
  defineMatrixAttributes();
  defineColorAttributes();
  defineObjectIdAttributes();

  if (object->texture != nullptr) {
    glTexture = OpenGLObject::createOpenGLTexture(object->texture, GL_TEXTURE7);
  }

  if (object->normalMap != nullptr) {
    glNormalMap = OpenGLObject::createOpenGLTexture(object->normalMap, GL_TEXTURE8);
  }

  if (object->shader != nullptr) {
    std::string path(object->shader);

    customShader = OpenGLObject::createShaderProgram(path);
  }
}

OpenGLObject::~OpenGLObject() {
  // TODO
}

void OpenGLObject::bindTextures() {
  if (glTexture != nullptr) {
    glTexture->use();
  }

  if (glNormalMap != nullptr) {
    glNormalMap->use();
  }
}

void OpenGLObject::bufferColorData() {
  auto* data = sourceObject->getColorBuffer();
  unsigned int size = sourceObject->getTotalInstances() * 3 * sizeof(float);

  bufferInstanceData(data, size, buffers[Buffer::COLOR]);
}

void OpenGLObject::bufferInstanceData(const void* data, unsigned int size, GLuint vbo) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void OpenGLObject::bufferMatrixData() {
  auto* data = sourceObject->getMatrixBuffer();
  unsigned int size = sourceObject->getTotalInstances() * 16 * sizeof(float);

  bufferInstanceData(data, size, buffers[Buffer::MATRIX]);
}

void OpenGLObject::bufferObjectIdData() {
  auto* data = sourceObject->getObjectIdBuffer();
  unsigned int size = sourceObject->getTotalInstances() * sizeof(int);

  bufferInstanceData(data, size, buffers[Buffer::ID]);
}

void OpenGLObject::bufferVertexData() {
  unsigned int totalVertices = sourceObject->getVertices().size();

  if (totalVertices == 0) {
    return;
  }

  unsigned int bufferSize = totalVertices * 11;
  float* buffer = new float[bufferSize];
  unsigned int i = 0;

  for (auto* vertex : sourceObject->getVertices()) {
    buffer[i++] = vertex->position.x;
    buffer[i++] = vertex->position.y;
    buffer[i++] = vertex->position.z;

    buffer[i++] = vertex->normal.x;
    buffer[i++] = vertex->normal.y;
    buffer[i++] = vertex->normal.z;

    buffer[i++] = vertex->tangent.x;
    buffer[i++] = vertex->tangent.y;
    buffer[i++] = vertex->tangent.z;

    buffer[i++] = vertex->uv.x;
    buffer[i++] = vertex->uv.y;
  }

  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), buffer, GL_STATIC_DRAW);

  delete[] buffer;
}

void OpenGLObject::bufferVertexElementData() {
  unsigned int totalFaces = sourceObject->getPolygons().size();

  if (totalFaces == 0) {
    return;
  }

  unsigned int bufferSize = totalFaces * 3;
  unsigned int* buffer = new unsigned int[bufferSize];
  unsigned int i = 0;

  for (auto* polygon : sourceObject->getPolygons()) {
    for (unsigned int v = 0; v < 3; v++) {
      buffer[i++] = polygon->vertices[v]->index;
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize * sizeof(unsigned int), buffer, GL_STATIC_DRAW);

  delete[] buffer;
}

OpenGLTexture* OpenGLObject::createOpenGLTexture(const Texture* texture, GLenum unit) {
  int id = texture->getId();

  if (OpenGLObject::textureMap.find(id) != OpenGLObject::textureMap.end()) {
    return OpenGLObject::textureMap.at(id);
  } else {
    auto* openGLTexture = new OpenGLTexture(texture, unit);

    OpenGLObject::textureMap.emplace(id, openGLTexture);

    return openGLTexture;
  }
}

ShaderProgram* OpenGLObject::createShaderProgram(std::string path) {
  if (OpenGLObject::shaderMap.find(path) != OpenGLObject::shaderMap.end()) {
    return OpenGLObject::shaderMap.at(path);
  } else {
    auto* program = new ShaderProgram();

    program->create();
    program->attachShader(ShaderLoader::loadVertexShader("./shaders/geometry.vertex.glsl"));
    program->attachShader(ShaderLoader::loadFragmentShader(path.c_str()));
    program->link();
    program->use();

    OpenGLObject::shaderMap.emplace(path, program);

    return program;
  }
}

void OpenGLObject::defineColorAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::COLOR]);

  glEnableVertexAttribArray(Attribute::MODEL_COLOR);
  glVertexAttribPointer(Attribute::MODEL_COLOR, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glVertexAttribDivisor(Attribute::MODEL_COLOR, 1);
}

void OpenGLObject::defineMatrixAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::MATRIX]);

  for (unsigned int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(Attribute::MODEL_MATRIX + i);
    glVertexAttribPointer(Attribute::MODEL_MATRIX + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
    glVertexAttribDivisor(Attribute::MODEL_MATRIX + i, 1);
  }
}

void OpenGLObject::defineObjectIdAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::ID]);

  glEnableVertexAttribArray(Attribute::OBJECT_ID);
  glVertexAttribIPointer(Attribute::OBJECT_ID, 1, GL_INT, sizeof(int), (void*)0);
  glVertexAttribDivisor(Attribute::OBJECT_ID, 1);
}

void OpenGLObject::defineVertexAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::VERTEX]);

  glEnableVertexAttribArray(Attribute::VERTEX_POSITION);
  glVertexAttribPointer(Attribute::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(Attribute::VERTEX_NORMAL);
  glVertexAttribPointer(Attribute::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

  glEnableVertexAttribArray(Attribute::VERTEX_TANGENT);
  glVertexAttribPointer(Attribute::VERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

  glEnableVertexAttribArray(Attribute::VERTEX_UV);
  glVertexAttribPointer(Attribute::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
}

void OpenGLObject::freeCachedResources() {
  for (auto [ key, glTexture ] : textureMap) {
    delete glTexture;
  }

  for (auto [ key, shader ] : shaderMap) {
    delete shader;
  }

  textureMap.clear();
  shaderMap.clear();
}

Object* OpenGLObject::getSourceObject() const {
  return sourceObject;
}

ShaderProgram* OpenGLObject::getCustomShader() const {
  return customShader;
}

bool OpenGLObject::hasCustomShader() const {
  return customShader != nullptr;
}

bool OpenGLObject::hasNormalMap() const {
  return glNormalMap != nullptr;
}

bool OpenGLObject::hasTexture() const {
  return glTexture != nullptr;
}

void OpenGLObject::render() {
  unsigned int totalRenderableInstances = sourceObject->getTotalRenderableInstances();

  if (totalRenderableInstances == 0) {
    return;
  }

  bindTextures();

  bufferMatrixData();
  bufferColorData();
  bufferObjectIdData();

  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glDrawElementsInstanced(GL_TRIANGLES, sourceObject->getPolygons().size() * 3, GL_UNSIGNED_INT, (void*)0, totalRenderableInstances);
}

std::map<int, OpenGLTexture*> OpenGLObject::textureMap;
std::map<std::string, ShaderProgram*> OpenGLObject::shaderMap;