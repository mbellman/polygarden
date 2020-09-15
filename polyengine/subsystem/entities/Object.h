#pragma once

#include <vector>
#include <functional>

#include "subsystem/entities/Entity.h"
#include "subsystem/Texture.h"
#include "subsystem/Geometry.h"
#include "subsystem/HeapList.h"

enum ObjectEffects {
  TREE_ANIMATION = 1 << 0,
  GRASS_ANIMATION = 1 << 1
};

// TODO: Split instancing logic into ReferenceObject + InstanceObject
class Object : public Entity {
public:
  Vec3f color = Vec3f(1.0f);
  Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f);
  const Texture* texture = nullptr;
  const Texture* normalMap = nullptr;
  const char* shader = nullptr;
  unsigned int effects = 0;
  unsigned int shadowCascadeLimit = 3;
  bool isEmissive = false;
  bool isReference = false;

  virtual ~Object();

  void clean();
  void disable();
  void enable();
  void enableInstances();
  void filterInstances(std::function<bool(Object*)> predicate);
  const float* getColorBuffer() const;
  const Matrix4& getMatrix() const;
  const float* getMatrixBuffer() const;
  const int* getObjectIdBuffer() const;
  const std::vector<Polygon*>& getPolygons() const;
  const Object* getReference() const;
  unsigned int getTotalEnabledInstances() const;
  unsigned int getTotalInstances() const;
  bool hasInstances() const;
  bool isDisabled() const;
  bool isInstance() const;
  void move(const Vec3f& movement);
  void rehydrate();
  void rotate(const Vec3f& rotation);
  void setColor(const Vec3f& color);
  void setOrientation(const Vec3f& orientation);
  void setPosition(const Vec3f& position);
  void setScale(const Vec3f& scale);
  void setScale(float scale);

protected:
  std::vector<Vertex3d*> vertices;
  std::vector<Polygon*> polygons;
  Matrix4 matrix;

  void addPolygon(int v1index, int v2index, int v3index);
  void addVertex(const Vec3f& position);
  void addVertex(const Vec3f& position, const Vec2f& uv);
  void setReference(Object* reference);
  void trackInstance(Object* instance);
  void untrackInstance(Object* instance);
  void updateNormals();

private:
  HeapList<Object> instances;
  Object* reference = this;
  float* matrixBuffer = nullptr;
  float* colorBuffer = nullptr;
  int* objectIdBuffer = nullptr;
  bool shouldReallocateBuffers = true;
  bool shouldRecomputeBuffers = false;
  bool isEnabled = true;

  void reallocateBuffers();
  void recomputeMatrix();
  void refreshColorBuffer();
  void refreshMatrixBuffer();
  void refreshObjectIdBuffer();
};