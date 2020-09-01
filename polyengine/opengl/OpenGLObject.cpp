#include "opengl/OpenGLObject.h"
#include "opengl/OpenGLTexture.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/Entities.h"

constexpr static unsigned int VERTEX_BUFFER = 0;
constexpr static unsigned int MATRIX_BUFFER = 0;

OpenGLObject::OpenGLObject(const Object* object) {
  sourceObject = object;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &mbo);
  glBindVertexArray(vao);

  bufferVertexData();

  defineVertexAttributes();
  defineMatrixAttributes();

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

void OpenGLObject::bindTextures() {
  if (glTexture != nullptr) {
    glTexture->use();
  }

  if (glNormalMap != nullptr) {
    glNormalMap->use();
  }
}

void OpenGLObject::bufferMatrixData() {
  auto totalInstances = sourceObject->getTotalInstances();
  auto* matrixBuffer = sourceObject->getMatrixBuffer();

  glBindBuffer(GL_ARRAY_BUFFER, mbo);
  glBufferData(GL_ARRAY_BUFFER, totalInstances * 16 * sizeof(float), matrixBuffer, GL_DYNAMIC_DRAW);
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

  // if (bufferSize < 10000) {
  //   printf("--------\n");

  //   for (int i = 0; i < bufferSize; i += 11) {
  //     printf("Position: %f, %f, %f | ", buffer[i], buffer[i + 1], buffer[i + 2]);
  //     printf("Normal: %f, %f, %f | ", buffer[i + 3], buffer[i + 4], buffer[i + 5]);
  //     printf("Tangent: %f, %f, %f | ", buffer[i + 6], buffer[i + 7], buffer[i + 8]);
  //     printf("UV: %f, %f\n", buffer[i + 9], buffer[i + 10]);
  //   }

  //   printf("--------\n");
  // }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

void OpenGLObject::defineMatrixAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, mbo);

  // mat4 modelMatrix
  for (unsigned int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(4 + i);
    glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
    glVertexAttribDivisor(4 + i, 1);
  }
}

void OpenGLObject::defineVertexAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // vec3 vertexPosition
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

  // vec3 vertexNormal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

  // vec3 vertexTangent
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

  // vec2 vertexUv
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
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
  if (sourceObject->isInstance()) {
    return;
  }

  unsigned int totalInstances = sourceObject->getTotalInstances();

  if (totalInstances == 0) {
    return;
  }

  bindTextures();
  bufferMatrixData();

  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, sourceObject->getPolygons().size() * 3, totalInstances);
}

std::map<int, OpenGLTexture*> OpenGLObject::textureMap;
std::map<std::string, ShaderProgram*> OpenGLObject::shaderMap;