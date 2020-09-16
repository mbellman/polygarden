#include "subsystem/entities/Skybox.h"

/**
 * Skybox
 * ------
 */
Skybox::Skybox() {
  this->isEmissive = true;

  for (int i = 0; i < 14; i++) {
    addVertex(Skybox::vertexPositions[i], Skybox::uvs[i]);
  }

  addPolygon(0, 1, 2);
  addPolygon(3, 4, 0);
  addPolygon(5, 6, 3);
  addPolygon(7, 8, 5);
  addPolygon(9, 3, 10);
  addPolygon(8, 11, 6);
  addPolygon(0, 4, 1);
  addPolygon(3, 6, 4);
  addPolygon(5, 8, 6);
  addPolygon(7, 12, 8);
  addPolygon(9, 5, 3);
  addPolygon(8, 13, 11);

  updateNormals();
}

Vec3f Skybox::vertexPositions[14] = {
  { -1.0f, -1.0f, -1.0f },
  { -1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, 1.0f },
  { 1.0f, -1.0f, -1.0f },
  { -1.0f, 1.0f, -1.0f },
  { 1.0f, -1.0f, 1.0f },
  { 1.0f, 1.0f, -1.0f },
  { -1.0f, -1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, 1.0f },
  { -1.0f, -1.0f, -1.0f },
  { -1.0f, 1.0f, -1.0f },
  { -1.0f, 1.0f, 1.0f },
  { -1.0f, 1.0f, 1.0f }
};

Vec2f Skybox::uvs[14] = {
  { 0.749978, 0.666474 },
  { 0.999934, 0.334000 },
  { 0.999934, 0.666474 },
  { 0.499000, 0.666474 },
  { 0.749978, 0.334000 },
  { 0.251000, 0.666476 },
  { 0.499000, 0.334000 },
  { 0.000108, 0.666476 },
  { 0.251000, 0.334000 },
  { 0.251000, 0.999427 },
  { 0.499000, 0.999427 },
  { 0.499000, 0.000572 },
  { 0.000108, 0.334000 },
  { 0.251000, 0.000572 }
};