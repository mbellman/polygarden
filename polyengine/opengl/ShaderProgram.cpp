#include <string>

#include "glew.h"
#include "glut.h"
#include "opengl/ShaderProgram.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLDebugger.h"

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(program);
}

void ShaderProgram::attachShader(GLuint shader) {
  glAttachShader(program, shader);
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

void ShaderProgram::setVec2f(std::string name, const Vec2f& value) const {
  glUniform2fv(getUniformLocation(name), 1, value.float2());
}

void ShaderProgram::setVec3f(std::string name, const Vec3f& value) const {
  glUniform3fv(getUniformLocation(name), 1, value.float3());
}

void ShaderProgram::use() const {
  glUseProgram(program);
}