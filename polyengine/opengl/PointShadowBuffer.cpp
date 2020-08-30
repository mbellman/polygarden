#include "opengl/PointShadowBuffer.h"
#include "opengl/ShaderLoader.h"

PointShadowBuffer::PointShadowBuffer() {
  createShaderPrograms();

  glScreenQuad = new OpenGLPipeline();

  pointShadowProgram.bindVertexInputs();
  glScreenQuad->createScreenQuad();
}

void PointShadowBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addDepthCubeMap(GL_TEXTURE3);
}

void PointShadowBuffer::createShaderPrograms() {
  VertexShaderInput geometryInputs[] = {
    { "vertexPosition", 3, GL_FLOAT },
    { "vertexNormal", 3, GL_FLOAT},
    { "vertexTangent", 3, GL_FLOAT },
    { "vertexUv", 2, GL_FLOAT }
  };

  VertexShaderInput quadInputs[] = {
    { "vertexPosition", 2, GL_FLOAT },
    { "vertexUv", 2, GL_FLOAT }
  };

  pointLightViewProgram.create();
  pointLightViewProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/point-lightview.vertex.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadGeometryShader("./polyengine/shaders/point-lightview.geometry.glsl"));
  pointLightViewProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/point-lightview.fragment.glsl"));
  pointLightViewProgram.link();
  pointLightViewProgram.use();
  pointLightViewProgram.setVertexInputs<float>(4, geometryInputs);

  pointShadowProgram.create();
  pointShadowProgram.attachShader(ShaderLoader::loadVertexShader("./polyengine/shaders/quad.vertex.glsl"));
  pointShadowProgram.attachShader(ShaderLoader::loadFragmentShader("./polyengine/shaders/point-shadowcaster.fragment.glsl"));
  pointShadowProgram.link();
  pointShadowProgram.use();
  pointShadowProgram.setVertexInputs<float>(2, quadInputs);
}

ShaderProgram& PointShadowBuffer::getPointLightViewProgram() {
  return pointLightViewProgram;
}

ShaderProgram& PointShadowBuffer::getPointShadowProgram() {
  return pointShadowProgram;
}