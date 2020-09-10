#include "subsystem/entities/Mesh.h"

/**
 * Mesh
 * ----
 */
void Mesh::displace(std::function<void(Vec3f&, int, int)> offsetHandler) {
  for (int i = 0; i < (height + 1); i++) {
    for (int j = 0; j < (width + 1); j++) {
      int idx = i * (width + 1) + j;

      offsetHandler(vertices[idx]->position, j, i);
    }
  }

  updateNormals();
}

void Mesh::setSize(int w, int h, float tileSize, Vec2f textureCycle) {
  width = w;
  height = h;

  // Add vertices
  Vec2f offset = {
    w * tileSize * -0.5f,
    h * tileSize * -0.5f
  };

  for (int i = 0; i < h + 1; i++) {
    for (int j = 0; j < w + 1; j++) {
      float x = j * tileSize + offset.x;
      float z = i * tileSize + offset.y;

      float u = (float)j / (float)w * textureCycle.x;
      float v = (float)i / (float)h * textureCycle.y;

      addVertex({ x, 0.0f, z}, { u, v });
    }
  }

  // Add polygons
  int totalPolygons = w * h * 2;

  for (int p = 0; p < totalPolygons; p++) {
    int tileIndex = p / 2;
    int rowIndex = tileIndex / w;
    int vOffset = rowIndex * (w + 1) + tileIndex % w;
    bool isTopPolygon = p % 2 == 0;
    int v1i = isTopPolygon ? vOffset : vOffset + 1;
    int v2i = vOffset + w + 1;
    int v3i = isTopPolygon ? vOffset + 1 : vOffset + w + 2;

    addPolygon(v1i, v2i, v3i);
  }

  updateNormals();
}