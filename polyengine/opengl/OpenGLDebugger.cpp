#include <cstdio>
#include <map>
#include <string>

#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/OpenGLDebugger.h"

static std::map<GLenum, std::string> ERROR_TYPES = {
  { GL_INVALID_ENUM, "GL_INVALID_ENUM "},
  { GL_INVALID_VALUE, "GL_INVALID_VALUE" },
  { GL_INVALID_OPERATION, "GL_INVALID_OPERATION" },
  { GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW" },
  { GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW" },
  { GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY" },
  { GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION" },
  { GL_CONTEXT_LOST, "GL_CONTEXT_LOST" },
  { GL_TABLE_TOO_LARGE, "GL_TABLE_TOO_LARGE" }
};

void OpenGLDebugger::checkErrors(const char* message) {
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("[OpenGLDebugger] (%s): Error %s\n", message, ERROR_TYPES[error]);
  }
}