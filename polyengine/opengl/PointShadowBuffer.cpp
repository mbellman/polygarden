#include "opengl/PointShadowBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

PointShadowBuffer::PointShadowBuffer() {
  createShaderPrograms();

  glScreenQuad = new OpenGLScreenQuad();
}

void PointShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addDepthCubeMap(GL_TEXTURE3);
}

void PointShadowBuffer::createShaderPrograms() {
  pointLightViewProgram.create();
  pointLightViewProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/point-lightview.vertex.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadGeometryShader("./polyengine/shaders/point-lightview.geometry.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/point-lightview.fragment.glsl"));
  pointLightViewProgram.link();

  pointShadowProgram.create();
  pointShadowProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/quad.vertex.glsl"));
  pointShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/point-shadowcaster.fragment.glsl"));
  pointShadowProgram.link();
}

ShaderProgram& PointShadowBuffer::getPointLightViewProgram() {
  return pointLightViewProgram;
}

ShaderProgram& PointShadowBuffer::getPointShadowProgram() {
  return pointShadowProgram;
}