#pragma once

#include <vector>
#include <string>

#include "glew.h"
#include "glut.h"
#include "subsystem/Math.h"

class ShaderProgram {
public:
  ~ShaderProgram();

  void attachShader(GLuint shader);
  void create();
  GLint getUniformLocation(const char* name) const;
  GLint getUniformLocation(std::string name) const;
  void link();
  void setBool(std::string name, bool value) const;
  void setFloat(std::string name, float value) const;
  void setInt(std::string name, int value) const;
  void setMatrix4(std::string name, const Matrix4& value) const;
  void setVec3f(std::string name, const Vec3f& value) const;
  void use() const;

private:
  GLuint program = -1;
};