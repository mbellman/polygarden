#pragma once

#include <vector>
#include <functional>

#include "subsystem/entities/Entity.h"
#include "subsystem/traits/Transformable.h"
#include "subsystem/Texture.h"
#include "subsystem/Geometry.h"
#include "subsystem/HeapList.h"

enum ObjectEffects {
  TREE_ANIMATION = 1 << 0,
  GRASS_ANIMATION = 1 << 1
};

class Instance;
class ReferenceMesh;

class Object : public Entity, public Transformable {
  friend class Instance;
  friend class ReferenceMesh;

public:
  Vec3f color = Vec3f(1.0f);
  Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f);
  const Texture* texture = nullptr;
  const Texture* normalMap = nullptr;
  const char* shader = nullptr;
  unsigned int effects = 0;
  unsigned int shadowCascadeLimit = 3;
  bool isEmissive = false;

  virtual ~Object();

  void disableRendering();
  void enableRendering();
  void enableRenderingAll();
  void enableRenderingWhere(std::function<bool(Object*)> predicate);
  const float* getColorBuffer() const;
  const Matrix4& getMatrix() const;
  const float* getMatrixBuffer() const;
  const int* getObjectIdBuffer() const;
  const std::vector<Polygon*>& getPolygons() const;
  const Object* getReference() const;
  unsigned int getTotalRenderableInstances() const;
  unsigned int getTotalInstances() const;
  const std::vector<Vertex3d*>& getVertices() const;
  bool hasInstances() const;
  bool isRenderable() const;
  void move(const Vec3f& movement);
  virtual void rehydrate();
  void rotate(const Vec3f& rotation);
  void setColor(const Vec3f& color);
  virtual void setOrientation(const Vec3f& orientation) override;
  virtual void setPosition(const Vec3f& position) override;
  virtual void setScale(const Vec3f& scale) override;

protected:
  std::vector<Vertex3d*> vertices;
  std::vector<Polygon*> polygons;
  Matrix4 matrix = Matrix4::identity();

  void addPolygon(int v1index, int v2index, int v3index);
  void addVertex(const Vec3f& position);
  void addVertex(const Vec3f& position, const Vec2f& uv);
  void trackInstance(Instance* instance);
  void untrackInstance(Instance* instance);
  void updateNormals();

private:
  HeapList<Instance> instances;
  Object* reference = this;
  bool isReference = false;
  float* matrixBuffer = nullptr;
  float* colorBuffer = nullptr;
  int* objectIdBuffer = nullptr;
  bool shouldReallocateBuffers = true;
  bool shouldRecomputeBuffers = false;
  bool isRenderingEnabled = true;

  void reallocateBuffers();
  void recomputeMatrix();
  void refreshColorBuffer();
  void refreshMatrixBuffer();
  void refreshObjectIdBuffer();
};