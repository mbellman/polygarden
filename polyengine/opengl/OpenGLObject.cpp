#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/Entities.h"

const static enum Buffer {
  VERTEX = 0,
  MATRIX = 1,
  COLOR = 2
};

const static enum Attribute {
  VERTEX_POSITION = 0,
  VERTEX_NORMAL = 1,
  VERTEX_TANGENT = 2,
  VERTEX_UV = 3,
  MODEL_COLOR = 4,
  MODEL_MATRIX = 5
};

OpenGLObject::OpenGLObject(const Object* object) {
  sourceObject = object;

  glGenVertexArrays(1, &vao);
  glGenBuffers(3, &buffers[0]);
  glBindVertexArray(vao);

  bufferVertexData();

  defineVertexAttributes();
  defineMatrixAttributes();
  defineColorAttributes();

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

void OpenGLObject::bufferInstanceData(const float* data, unsigned int size, GLuint vbo) {
  unsigned int totalInstances = sourceObject->getTotalInstances();

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  if (previousTotalInstances != totalInstances) {
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  } else {
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
  }
}

void OpenGLObject::bufferMatrixData() {
  auto* data = sourceObject->getMatrixBuffer();
  unsigned int size = sourceObject->getTotalInstances() * 16 * sizeof(float);

  bufferInstanceData(data, size, buffers[Buffer::MATRIX]);
}

void OpenGLObject::bufferVertexData() {
  unsigned int totalVertices = sourceObject->getPolygons().size() * 3;

  if (totalVertices == 0) {
    return;
  }

  unsigned int bufferSize = totalVertices * 11;
  float* buffer = new float[bufferSize];
  unsigned int i = 0;

  for (auto* polygon : sourceObject->getPolygons()) {
    for (int v = 0; v < 3; v++) {
      const Vertex3d& vertex = *polygon->vertices[v];

      buffer[i++] = vertex.position.x;
      buffer[i++] = vertex.position.y;
      buffer[i++] = vertex.position.z;

      buffer[i++] = vertex.normal.x;
      buffer[i++] = vertex.normal.y;
      buffer[i++] = vertex.normal.z;

      buffer[i++] = vertex.tangent.x;
      buffer[i++] = vertex.tangent.y;
      buffer[i++] = vertex.tangent.z;

      buffer[i++] = vertex.uv.x;
      buffer[i++] = vertex.uv.y;
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), buffer, GL_STATIC_DRAW);

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
    program->attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/geometry.vertex.glsl"));
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

const Object* OpenGLObject::getSourceObject() const {
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
  unsigned int totalEnabledInstances = sourceObject->getTotalEnabledInstances();

  if (totalEnabledInstances > 0) {
    bindTextures();
    bufferMatrixData();
    bufferColorData();

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, sourceObject->getPolygons().size() * 3, totalEnabledInstances);
  }

  previousTotalInstances = sourceObject->getTotalInstances();
}

std::map<int, OpenGLTexture*> OpenGLObject::textureMap;
std::map<std::string, ShaderProgram*> OpenGLObject::shaderMap;