#include <cstdio>

#include "glew.h"
#include "SDL_opengl.h"
#include "glut.h"
#include "opengl/OpenGLDebugger.h"

void OpenGLDebugger::checkErrors(const char* message) {
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("OpenGL Error [%s]: %d\n", message, error);
  }
}