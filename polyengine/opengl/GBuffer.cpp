#include "opengl/GBuffer.h"
#include "opengl/ShaderLoader.h"
#include "opengl/OpenGLScreenQuad.h"

GBuffer::GBuffer() {
  createShaderPrograms();

  glScreenQuad = new OpenGLScreenQuad();
}

GBuffer::~GBuffer() {
  // TODO
}

void GBuffer::createFrameBuffer(unsigned int width, unsigned int height) {
  if (frameBuffer != nullptr) {
    delete frameBuffer;
  }

  frameBuffer = new FrameBuffer(width, height);

  frameBuffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_BORDER, GL_TEXTURE0);     // Color
  frameBuffer->addColorTexture(GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_BORDER, GL_TEXTURE1);   // Normal/depth
  frameBuffer->addColorTexture(GL_RGB32F, GL_RGB, GL_CLAMP_TO_BORDER, GL_TEXTURE2);     // Position
  frameBuffer->addDepthStencilBuffer();
  frameBuffer->bindColorTextures();
}

void GBuffer::createShaderPrograms() {
  geometryProgram.create();
  geometryProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/geometry.vertex.glsl"));
  geometryProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/geometry.fragment.glsl"));
  geometryProgram.link();

  illuminationProgram.create();
  illuminationProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  illuminationProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/illumination.fragment.glsl"));
  illuminationProgram.link();

  albedoProgram.create();
  albedoProgram.attachShader(ShaderLoader::loadVertexShader("./shaders/quad.vertex.glsl"));
  albedoProgram.attachShader(ShaderLoader::loadFragmentShader("./shaders/albedo.fragment.glsl"));
  albedoProgram.link();
}

ShaderProgram& GBuffer::getShaderProgram(GBuffer::Shader shader) {
  switch (shader) {
    case GBuffer::Shader::GEOMETRY:
      return geometryProgram;
    case GBuffer::Shader::ILLUMINATION:
      return illuminationProgram;
    case GBuffer::Shader::ALBEDO:
      return albedoProgram;
    default:
      return geometryProgram;
  }
}

void GBuffer::writeToAllBuffers() {
  frameBuffer->bindColorTextures();
}