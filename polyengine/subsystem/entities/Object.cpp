#include "subsystem/entities/Object.h"
#include "subsystem/entities/Instance.h"

/**
 * Object
 * ------
 */
Object::~Object() {
  if (matrixBuffer != nullptr) {
    delete[] matrixBuffer;
  }

  if (colorBuffer != nullptr) {
    delete[] colorBuffer;
  }

  if (objectIdBuffer != nullptr) {
    delete[] objectIdBuffer;
  }

  for (auto* polygon : polygons) {
    delete polygon;
  }

  for (auto* vertex : vertices) {
    delete vertex;
  }

  for (auto* instance : instances) {
    instance->expire();
  }

  polygons.clear();
  vertices.clear();
  instances.clear();
}

void Object::addPolygon(int v1index, int v2index, int v3index) {
  Polygon* polygon = new Polygon();

  polygon->vertices[0] = vertices[v1index];
  polygon->vertices[1] = vertices[v2index];
  polygon->vertices[2] = vertices[v3index];

  vertices[v1index]->polygons.push_back(polygon);
  vertices[v2index]->polygons.push_back(polygon);
  vertices[v3index]->polygons.push_back(polygon);

  polygons.push_back(polygon);
}

void Object::addVertex(const Vec3f& position) {
  addVertex(position, { 0.0f, 0.0f });
}

void Object::addVertex(const Vec3f& position, const Vec2f& uv) {
  Vertex3d* vertex = new Vertex3d();

  vertex->position = position;
  vertex->uv = uv;
  vertex->index = vertices.size();

  vertices.push_back(vertex);
}

void Object::disableRendering() {
  if (isRenderingEnabled) {
    isRenderingEnabled = false;
    reference->shouldRecomputeBuffers = true;
  }
}

void Object::enableRendering() {
  if (!isRenderingEnabled) {
    isRenderingEnabled = true;
    reference->shouldRecomputeBuffers = true;
  }
}

void Object::enableRenderingAll() {
  enableRendering();

  for (auto* instance : instances) {
    instance->enableRendering();
  }
}

void Object::enableRenderingWhere(std::function<bool(Object*)> predicate) {
  if (predicate(this)) {
    enableRendering();
  } else {
    disableRendering();
  }

  for (auto* instance : instances) {
    if (predicate(instance)) {
      instance->enableRendering();
    } else {
      instance->disableRendering();
    }
  }
}

const float* Object::getColorBuffer() const {
  return colorBuffer;
}

const Matrix4& Object::getMatrix() const {
  return matrix;
}

const float* Object::getMatrixBuffer() const {
  return matrixBuffer;
}

const int* Object::getObjectIdBuffer() const {
  return objectIdBuffer;
}

const std::vector<Polygon*>& Object::getPolygons() const {
  return polygons;
}

const Object* Object::getReference() const {
  return reference;
}

unsigned int Object::getTotalRenderableInstances() const {
  if (!hasInstances() && !isReference) {
    return isRenderable() ? 1 : 0;
  }

  unsigned int total = 0;

  for (auto* instance : instances) {
    if (instance->isRenderingEnabled) {
      total++;
    }
  }

  return total;
}

unsigned int Object::getTotalInstances() const {
  return (
    hasInstances()
      ? instances.length() :
    isReference
      ? 0 :
    1
  );
}

const std::vector<Vertex3d*>& Object::getVertices() const {
  return vertices;
}

bool Object::hasInstances() const {
  return instances.length() > 0;
}

bool Object::isRenderable() const {
  return isRenderingEnabled;
}

void Object::move(const Vec3f& movement) {
  setPosition(position + movement);
}

void Object::reallocateBuffers() {
  if (matrixBuffer != nullptr) {
    delete[] matrixBuffer;
  }

  if (colorBuffer != nullptr) {
    delete[] colorBuffer;
  }

  if (objectIdBuffer != nullptr) {
    delete[] objectIdBuffer;
  }

  colorBuffer = new float[getTotalInstances() * 3];
  matrixBuffer = new float[getTotalInstances() * 16];
  objectIdBuffer = new int[getTotalInstances()];
}

void Object::recomputeMatrix() {
  matrix = (
    Matrix4::translate({ position.x, position.y, -1.0f * position.z }) *
    Matrix4::rotate(orientation) *
    Matrix4::scale(scale)
  ).transpose();

  reference->shouldRecomputeBuffers = true;
}

void Object::refreshColorBuffer() {
  if (hasInstances()) {
    unsigned int idx = 0;

    for (unsigned int i = 0; i < instances.length(); i++) {
      auto* instance = instances[i];

      if (instance->isRenderingEnabled) {
        colorBuffer[idx * 3] = instances[i]->color.x;
        colorBuffer[idx * 3 + 1] = instances[i]->color.y;
        colorBuffer[idx * 3 + 2] = instances[i]->color.z;

        idx++;
      }
    }
  } else {
    colorBuffer[0] = color.x;
    colorBuffer[1] = color.y;
    colorBuffer[2] = color.z;
  }
}

void Object::refreshMatrixBuffer() {
  if (hasInstances()) {
    unsigned int idx = 0;

    for (unsigned int i = 0; i < instances.length(); i++) {
      auto* instance = instances[i];

      if (instance->isRenderingEnabled) {
        const Matrix4& matrix = instance->getMatrix();

        memcpy(&matrixBuffer[idx++ * 16], matrix.m, 16 * sizeof(float));
      }
    }
  } else {
    memcpy(matrixBuffer, matrix.m, 16 * sizeof(float));
  }
}

void Object::refreshObjectIdBuffer() {
  if (hasInstances()) {
    unsigned int idx = 0;

    for (unsigned int i = 0; i < instances.length(); i++) {
      auto* instance = instances[i];

      if (instance->isRenderingEnabled) {
        objectIdBuffer[idx++] = (int)i;
      }
    }
  } else {
    objectIdBuffer[0] = id;
  }
}

void Object::rehydrate() {
  if (getTotalInstances() > 0) {
    if (shouldReallocateBuffers) {
      reallocateBuffers();
    }

    if (shouldRecomputeBuffers) {
      refreshMatrixBuffer();
      refreshColorBuffer();
      refreshObjectIdBuffer();
    }
  }

  shouldReallocateBuffers = false;
  shouldRecomputeBuffers = false;
}

void Object::rotate(const Vec3f& rotation) {
  orientation += rotation;

  recomputeMatrix();
}

void Object::setColor(const Vec3f& color) {
  this->color = color;
  reference->shouldRecomputeBuffers = true;
}

void Object::setOrientation(const Vec3f& orientation) {
  this->orientation = orientation;

  recomputeMatrix();
}

void Object::setPosition(const Vec3f& position) {
  this->position = position;

  recomputeMatrix();
}

void Object::setScale(const Vec3f& scale) {
  this->scale = scale;

  recomputeMatrix();
}

void Object::setScale(float scale) {
  setScale(Vec3f(scale));
}

void Object::trackInstance(Instance* instance) {
  instances.push(instance);

  shouldReallocateBuffers = true;
  shouldRecomputeBuffers = true;
}

void Object::untrackInstance(Instance* instance) {
  instances.remove(instance);

  shouldReallocateBuffers = true;
  shouldRecomputeBuffers = true;
}

void Object::updateNormals() {
  for (auto* polygon : polygons) {
    polygon->updateNormal();
    polygon->updateTangent();
  }

  for (auto* vertex : vertices) {
    vertex->updateNormal();
    vertex->updateTangent();
  }
}