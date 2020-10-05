#include "opengl/OpenGLPointShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

OpenGLPointShadowBuffer::OpenGLPointShadowBuffer() {
  createShaderPrograms();
}

void OpenGLPointShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addDepthCubeMap(GL_TEXTURE3);
}

void OpenGLPointShadowBuffer::createShaderPrograms() {
  lightViewProgram.create();
  lightViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/point-lightview.vertex.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadGeometryShader("./shaders/point-lightview.geometry.glsl"));
  lightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/point-lightview.fragment.glsl"));
  lightViewProgram.link();

  cameraViewProgram.create();
  cameraViewProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  cameraViewProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/point-shadowcaster.fragment.glsl"));
  cameraViewProgram.link();
}

ShaderProgram& OpenGLPointShadowBuffer::getCameraViewProgram() {
  return cameraViewProgram;
}

ShaderProgram& OpenGLPointShadowBuffer::getLightViewProgram() {
  return lightViewProgram;
}