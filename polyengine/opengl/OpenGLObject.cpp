#include <algorithm>

#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/PerformanceProfiler.h"

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

  addLod(object);

  if (object->texture != nullptr) {
    glTexture = OpenGLObject::createOpenGLTexture(object->texture, GL_TEXTURE7);
  }

  if (object->normalMap != nullptr) {
    glNormalMap = OpenGLObject::createOpenGLTexture(object->normalMap, GL_TEXTURE8);
  }

  if (object->shadowLod != nullptr) {
    addLod(object->shadowLod);
    setActiveLodIndex(0);
  }
}

OpenGLObject::~OpenGLObject() {
  for (auto* glLod : glLods) {
    // TODO: Delete VAO/EBO/VBOs

    delete glLod;
  }

  glLods.clear();
}

void OpenGLObject::addLod(const Object* object) {
  auto* glLod = new OpenGLObjectLod();

  glGenVertexArrays(1, &glLod->vao);
  glGenBuffers(4, &glLod->buffers[0]);
  glGenBuffers(1, &glLod->ebo);
  glBindVertexArray(glLod->vao);

  glLod->baseObject = object;

  glLods.push_back(glLod);
  setActiveLodIndex(glLods.size() - 1);

  bufferVertexData();
  bufferVertexElementData();

  defineVertexAttributes();
  defineMatrixAttributes();
  defineColorAttributes();
  defineObjectIdAttributes();
}

void OpenGLObject::bindTextures() {
  if (glTexture != nullptr) {
    glTexture->use();
  }

  if (glNormalMap != nullptr) {
    glNormalMap->use();
  }
}

void OpenGLObject::bufferDynamicData(const void* data, unsigned int size, GLuint vbo) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void OpenGLObject::bufferInstanceData() {
  unsigned int totalInstances = sourceObject->getTotalInstances();
  const float* matrices = sourceObject->getMatrixBuffer();
  const float* colors = sourceObject->getColorBuffer();
  const int* objectIds = sourceObject->getObjectIdBuffer();
  auto* glLod = getActiveLod();

  bufferDynamicData(matrices, totalInstances * 16 * sizeof(float), glLod->buffers[Buffer::MATRIX]);
  bufferDynamicData(colors, totalInstances * 3 * sizeof(float), glLod->buffers[Buffer::COLOR]);
  bufferDynamicData(objectIds, totalInstances * sizeof(int), glLod->buffers[Buffer::ID]);
}

void OpenGLObject::bufferVertexData() {
  auto* glLod = getActiveLod();
  unsigned int totalVertices = glLod->baseObject->getVertices().size();

  if (totalVertices == 0) {
    return;
  }

  unsigned int bufferSize = totalVertices * 11;
  float* buffer = new float[bufferSize];
  unsigned int i = 0;

  for (auto* vertex : glLod->baseObject->getVertices()) {
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

  glBindBuffer(GL_ARRAY_BUFFER, glLod->buffers[Buffer::VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), buffer, GL_STATIC_DRAW);

  delete[] buffer;
}

void OpenGLObject::bufferVertexElementData() {
  auto* glLod = getActiveLod();
  unsigned int totalPolygons = glLod->baseObject->getPolygons().size();

  if (totalPolygons == 0) {
    return;
  }

  unsigned int bufferSize = totalPolygons * 3;
  unsigned int* buffer = new unsigned int[bufferSize];
  unsigned int i = 0;

  for (auto* polygon : glLod->baseObject->getPolygons()) {
    for (unsigned int v = 0; v < 3; v++) {
      buffer[i++] = polygon->vertices[v]->index;
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glLod->ebo);
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

void OpenGLObject::defineColorAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, getActiveLod()->buffers[Buffer::COLOR]);

  glEnableVertexAttribArray(Attribute::MODEL_COLOR);
  glVertexAttribPointer(Attribute::MODEL_COLOR, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glVertexAttribDivisor(Attribute::MODEL_COLOR, 1);
}

void OpenGLObject::defineMatrixAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, getActiveLod()->buffers[Buffer::MATRIX]);

  for (unsigned int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(Attribute::MODEL_MATRIX + i);
    glVertexAttribPointer(Attribute::MODEL_MATRIX + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
    glVertexAttribDivisor(Attribute::MODEL_MATRIX + i, 1);
  }
}

void OpenGLObject::defineObjectIdAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, getActiveLod()->buffers[Buffer::ID]);

  glEnableVertexAttribArray(Attribute::OBJECT_ID);
  glVertexAttribIPointer(Attribute::OBJECT_ID, 1, GL_INT, sizeof(int), (void*)0);
  glVertexAttribDivisor(Attribute::OBJECT_ID, 1);
}

void OpenGLObject::defineVertexAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, getActiveLod()->buffers[Buffer::VERTEX]);

  glEnableVertexAttribArray(Attribute::VERTEX_POSITION);
  glVertexAttribPointer(Attribute::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(Attribute::VERTEX_NORMAL);
  glVertexAttribPointer(Attribute::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

  glEnableVertexAttribArray(Attribute::VERTEX_TANGENT);
  glVertexAttribPointer(Attribute::VERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

  glEnableVertexAttribArray(Attribute::VERTEX_UV);
  glVertexAttribPointer(Attribute::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
}

OpenGLObjectLod* OpenGLObject::getActiveLod() {
  return glLods[activeLodIndex];
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

  auto* glLod = getActiveLod();

  bindTextures();
  bufferInstanceData();

  glBindVertexArray(glLod->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glLod->ebo);
  glDrawElementsInstanced(GL_TRIANGLES, glLod->baseObject->getPolygons().size() * 3, GL_UNSIGNED_INT, (void*)0, totalRenderableInstances);

  PerformanceProfiler::trackObject(sourceObject, totalRenderableInstances);
  PerformanceProfiler::trackDrawCall();
}

void OpenGLObject::renderLod(unsigned int index) {
  setActiveLodIndex(index);
  render();
}

void OpenGLObject::renderShadowLod() {
  unsigned int previousActiveLodIndex = activeLodIndex;

  renderLod(glLods.size() - 1);
  setActiveLodIndex(previousActiveLodIndex);
}

void OpenGLObject::setActiveLodIndex(unsigned int index) {
  activeLodIndex = std::min((int)index, (int)glLods.size() - 1);
}

std::map<int, OpenGLTexture*> OpenGLObject::textureMap;
std::map<std::string, ShaderProgram*> OpenGLObject::shaderMap;