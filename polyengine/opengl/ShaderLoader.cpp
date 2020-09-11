#include <string>
#include <map>
#include <cstdio>

#include "glew.h"
#include "opengl/ShaderLoader.h"
#include "subsystem/FileLoader.h"

const std::string SHADER_DIRECTORY = "shaders/";
const std::string INCLUDE_DIRECTIVE = "#include <";

GLuint ShaderLoader::load(GLenum shaderType, const char* path) {
  GLuint shader = glCreateShader(shaderType);

  std::string source = FileLoader::load(path);
  std::map<std::string, bool> includeMap;
  std::size_t includeStart;

  while ((includeStart = source.find(INCLUDE_DIRECTIVE)) != std::string::npos) {
    int pathStart = includeStart + INCLUDE_DIRECTIVE.length();
    int pathEnd = source.find(">", pathStart) - pathStart;
    int includeEnd = source.find("\n", includeStart) - includeStart;
    std::string path = source.substr(pathStart, pathEnd);
    bool isUniquePath = includeMap.find(path) == includeMap.end();

    if (isUniquePath) {
      std::string includeSource = FileLoader::load((SHADER_DIRECTORY + path).c_str());

      includeMap.emplace(path, true);

      source.replace(includeStart, includeEnd, includeSource);
    } else {
      source.replace(includeStart, includeEnd, "");
    }
  }

  includeMap.clear();

  const GLchar* shaderSource = source.c_str();

  glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char error[512];

    glGetShaderInfoLog(shader, 512, 0, error);
    printf("[ShaderLoader] Failed to compile shader: %s\n", path);
    printf("%s\n", error);
  }

  return shader;
}

GLuint ShaderLoader::loadFragmentShader(const char* path) {
  return load(GL_FRAGMENT_SHADER, path);
}

GLuint ShaderLoader::loadGeometryShader(const char* path) {
  return load(GL_GEOMETRY_SHADER, path);
}

GLuint ShaderLoader::loadVertexShader(const char* path) {
  return load(GL_VERTEX_SHADER, path);
}