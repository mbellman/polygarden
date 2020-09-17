#pragma once

#include <vector>

#include "subsystem/Math.h"

struct Polygon;

/**
 * A 3D polygon vertex.
 */
struct Vertex3d {
  Vertex3d() {};

  Vec3f position;
  Vec3f normal;
  Vec3f tangent;
  Vec2f uv;
  std::vector<Polygon*> polygons;
  unsigned int index = 0;

  void updateNormal();
  void updateTangent();
};

/**
 * A polygon consisting of three 3D vertices. Since
 * polygons can share vertices, each vertex is held
 * by reference.
 */
struct Polygon {
  Polygon() {};
  Polygon(Vertex3d* v1, Vertex3d* v2, Vertex3d* v3);

  Vertex3d* vertices[3];
  Vec3f normal;
  Vec3f tangent;

  void updateNormal();
  void updateTangent();
};