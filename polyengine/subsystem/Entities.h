#pragma once

#include <vector>
#include <functional>
#include <string>

#include "subsystem/Math.h"
#include "subsystem/Geometry.h"
#include "subsystem/ObjLoader.h"
#include "subsystem/Texture.h"
#include "subsystem/HeapList.h"

enum ObjectEffects {
  TREE_ANIMATION = 1 << 0,
  GRASS_ANIMATION = 1 << 1
};

struct Entity {
  Entity();
  virtual ~Entity() {};

  static int total;
  int id;
  Vec3f position;
  Vec3f orientation;
  float lifetime = -1.0f;
  std::function<void(float)> onUpdate = nullptr;

  void expire();
  bool isExpired() const;

  template<typename T>
  bool isOfType() {
    return dynamic_cast<T*>(this) != 0;
  }
};

struct Light : Entity {
  enum LightType {
    POINT = 0,
    DIRECTIONAL = 1,
    SPOTLIGHT = 2
  };

  Light() {};
  Light(const Vec3f& position, const Vec3f& color, float radius);

  LightType type = LightType::POINT;
  Vec3f color = Vec3f(1.0f);
  Vec3f direction;
  float radius = 100.0f;
  float power = 1.0f;
  bool canCastShadows = false;
};

class Camera : public Entity {
public:
  // TODO: Create 'Orientation' struct with below methods
  Vec3f getDirection() const;
  Vec3f getLeftDirection() const;
  Vec3f getOrientationDirection(const Vec3f& orientation) const;
  Vec3f getRightDirection() const;
};

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

class Mesh : public Object {
public:
  void displace(std::function<void(Vec3f&, int, int)> offsetHandler);
  void setSize(int w, int h, float tileSize, Vec2f textureCycle = Vec2f(1.0f, 1.0f));

private:
  int width;
  int height;
};

class Cube : public Object {
public:
  Cube();

private:
  static Vec3f corners[8];
  static Vec2f uvs[4];
  static int faces[6][4];
};

class Skybox : public Object {
public:
  void from(const Texture* texture);

private:
  static Vec3f vertexPositions[14];
  static Vec2f uvs[14];
};

class Model : public Object {
public:
  void from(const ObjLoader& loader);
  void from(Model* reference);

private:
  void buildTexturedModel(const ObjLoader& loader);
  void buildUntexturedModel(const ObjLoader& loader);
};