#include "opengl/OpenGLLightingQuad.h"
#include "subsystem/entities/Camera.h"
#include "subsystem/Math.h"

const float QUAD_DATA[] = {
  -1.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f,
  1.0f, -1.0f, 1.0f, 0.0f
};

const static enum Buffer {
  QUAD_VERTEX,
  LIGHT,
  QUAD_TRANSFORM
};

const static enum Attribute {
  VERTEX_POSITION = 0,
  VERTEX_UV = 1,
  QUAD_OFFSET = 2,
  QUAD_SCALE = 3,
  LIGHT_POSITION = 4,
  LIGHT_DIRECTION = 5,
  LIGHT_COLOR = 6,
  LIGHT_RADIUS = 7,
  LIGHT_TYPE = 8
};

struct LightData {
  float position[3];
  float direction[3];
  float color[3];
  float radius;
  int type;
};

struct QuadTransformData {
  float offset[2];
  float scale[2];
};

OpenGLLightingQuad::OpenGLLightingQuad() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(3, &buffers[0]);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::QUAD_VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), QUAD_DATA, GL_STATIC_DRAW);

  defineQuadVertexAttributes();
  defineLightAttributes();
  defineQuadTransformAttributes();
}

OpenGLLightingQuad::~OpenGLLightingQuad() {
  // TODO
}

// TODO: Perform some of this work in OpenGLIlluminator, or rename
// OpenGLLightingQuad to better represent its responsibility over
// screen-space light bounds calculation
void OpenGLLightingQuad::bufferData(const std::vector<Light*>& lights) {
  LightData* lightBuffer = new LightData[lights.size()];
  QuadTransformData* transformBuffer = new QuadTransformData[lights.size()];
  Matrix4 projection = Matrix4::projection({ 0, 0, 1920, 1080 }, Camera::active->fov * 0.5f, 1.0f, 10000.0f);
  Matrix4 view = Camera::active->getViewMatrix();
  float aspectRatio = 1920.0f / 1080.0f;

  for (unsigned int i = 0; i < lights.size(); i++) {
    auto* light = lights[i];

    memcpy(lightBuffer[i].position, &light->position, 3 * sizeof(float));
    memcpy(lightBuffer[i].direction, &light->direction, 3 * sizeof(float));
    memcpy(lightBuffer[i].color, &(light->color * light->power), 3 * sizeof(float));

    lightBuffer[i].radius = light->radius;
    lightBuffer[i].type = light->type;

    Vec3f localLightPosition = view * light->position;

    if (localLightPosition.z > 0.0f && light->type != Light::LightType::DIRECTIONAL) {
      Vec3f clip = (projection * localLightPosition) / localLightPosition.z;

      transformBuffer[i].offset[0] = clip.x;
      transformBuffer[i].offset[1] = clip.y;
      transformBuffer[i].scale[0] = light->radius / localLightPosition.z * aspectRatio;
      transformBuffer[i].scale[1] = light->radius / localLightPosition.z * 1.2f;
    } else {
      float scale = (localLightPosition.magnitude() < light->radius * 0.5f || light->type == Light::LightType::DIRECTIONAL) ? 1.0f : 0.0f;

      transformBuffer[i].offset[0] = 0.0f;
      transformBuffer[i].offset[1] = 0.0f;
      transformBuffer[i].scale[0] = scale;
      transformBuffer[i].scale[1] = scale;
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::LIGHT]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(LightData) * lights.size(), lightBuffer, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::QUAD_TRANSFORM]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QuadTransformData) * lights.size(), transformBuffer, GL_DYNAMIC_DRAW);

  delete[] lightBuffer;
  delete[] transformBuffer;
}

void OpenGLLightingQuad::defineLightAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::LIGHT]);

  glEnableVertexAttribArray(Attribute::LIGHT_POSITION);
  glVertexAttribPointer(Attribute::LIGHT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(LightData), (void*)offsetof(LightData, position));
  glVertexAttribDivisor(Attribute::LIGHT_POSITION, 1);

  glEnableVertexAttribArray(Attribute::LIGHT_DIRECTION);
  glVertexAttribPointer(Attribute::LIGHT_DIRECTION, 3, GL_FLOAT, GL_FALSE, sizeof(LightData), (void*)offsetof(LightData, direction));
  glVertexAttribDivisor(Attribute::LIGHT_DIRECTION, 1);

  glEnableVertexAttribArray(Attribute::LIGHT_COLOR);
  glVertexAttribPointer(Attribute::LIGHT_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(LightData), (void*)offsetof(LightData, color));
  glVertexAttribDivisor(Attribute::LIGHT_COLOR, 1);

  glEnableVertexAttribArray(Attribute::LIGHT_RADIUS);
  glVertexAttribPointer(Attribute::LIGHT_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof(LightData), (void*)offsetof(LightData, radius));
  glVertexAttribDivisor(Attribute::LIGHT_RADIUS, 1);

  glEnableVertexAttribArray(Attribute::LIGHT_TYPE);
  glVertexAttribIPointer(Attribute::LIGHT_TYPE, 1, GL_INT, sizeof(LightData), (void*)offsetof(LightData, type));
  glVertexAttribDivisor(Attribute::LIGHT_TYPE, 1);
}

void OpenGLLightingQuad::defineQuadTransformAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::QUAD_TRANSFORM]);

  glEnableVertexAttribArray(Attribute::QUAD_OFFSET);
  glVertexAttribPointer(Attribute::QUAD_OFFSET, 2, GL_FLOAT, GL_FALSE, sizeof(QuadTransformData), (void*)offsetof(QuadTransformData, offset));
  glVertexAttribDivisor(Attribute::QUAD_OFFSET, 1);

  glEnableVertexAttribArray(Attribute::QUAD_SCALE);
  glVertexAttribPointer(Attribute::QUAD_SCALE, 2, GL_FLOAT, GL_FALSE, sizeof(QuadTransformData), (void*)offsetof(QuadTransformData, scale));
  glVertexAttribDivisor(Attribute::QUAD_SCALE, 1);
}

void OpenGLLightingQuad::defineQuadVertexAttributes() {
  glBindBuffer(GL_ARRAY_BUFFER, buffers[Buffer::QUAD_VERTEX]);

  glEnableVertexAttribArray(Attribute::VERTEX_POSITION);
  glVertexAttribPointer(Attribute::VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

  glEnableVertexAttribArray(Attribute::VERTEX_UV);
  glVertexAttribPointer(Attribute::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void OpenGLLightingQuad::render(const std::vector<Light*>& lights) {
  if (lights.size() == 0) {
    return;
  }

  bufferData(lights);

  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, lights.size());
}