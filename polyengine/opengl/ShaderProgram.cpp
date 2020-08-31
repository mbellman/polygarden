#include <string>

#include "glew.h"
#include "glut.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLPipeline.h"
#include "opengl/OpenGLDebugger.h"

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(program);
}

void ShaderProgram::attachShader(GLuint shader) {
  glAttachShader(program, shader);
}

void ShaderProgram::bindInputs(OpenGLPipeline* glPipeline) {
  glPipeline->bindVBO();

  for (auto& attribute : vertexAttributes) {
    setVertexAttribute(attribute);
  }

  glPipeline->bindMBO();

  for (auto& attribute : matrixAttributes) {
    setMatrixAttribute(attribute);
  }

  glBindVertexArray(0);
}

void ShaderProgram::create() {
  program = glCreateProgram();
}

GLint ShaderProgram::getUniformLocation(const char* name) const {
  return glGetUniformLocation(program, name);
}

GLint ShaderProgram::getUniformLocation(std::string name) const {
  return glGetUniformLocation(program, name.c_str());
}

void ShaderProgram::link() {
  glLinkProgram(program);
}

void ShaderProgram::setBool(std::string name, bool value) const {
  setInt(name, value);
}

void ShaderProgram::setFloat(std::string name, float value) const {
  glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setInt(std::string name, int value) const {
  glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setMatrix4(std::string name, const Matrix4& value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value.m);
}

void ShaderProgram::setMatrixAttribute(const MatrixAttribute& attribute) {
  GLint location = glGetAttribLocation(program, attribute.name);

  if (location != -1) {
    for (unsigned int i = 0; i < 4; i++) {
      glEnableVertexAttribArray(location + i);
      OpenGLDebugger::checkErrors("setMatrixAttribute: glEnableVertexAttribArray");
      glVertexAttribPointer(location + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
      OpenGLDebugger::checkErrors("setMatrixAttribute: glVertexAttribPointer");
      glVertexAttribDivisor(location + i, 1);
      OpenGLDebugger::checkErrors("setMatrixAttribute: glVertexAttribDivisor");
    }
  }
}

void ShaderProgram::setMatrixInput(const char* name) {
  MatrixAttribute attribute;

  attribute.name = name;

  matrixAttributes.push_back(attribute);
}

void ShaderProgram::setVec3f(std::string name, const Vec3f& value) const {
  glUniform3fv(getUniformLocation(name), 1, value.float3());
}

void ShaderProgram::setVertexAttribute(const VertexAttribute& attribute) {
  GLint location = glGetAttribLocation(program, attribute.name);

  if (location != -1) {
    glEnableVertexAttribArray(location);
    OpenGLDebugger::checkErrors("setVertexAttribute: glEnableVertexAttribArray");
    glVertexAttribPointer(location, attribute.size, attribute.type, GL_FALSE, attribute.stride, (void*)attribute.offset);
    OpenGLDebugger::checkErrors("setVertexAttribute: glVertexAttribPointer");
  }
}

void ShaderProgram::use() const {
  glUseProgram(program);
}