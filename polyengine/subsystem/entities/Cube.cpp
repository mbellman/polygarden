#include "subsystem/entities/Cube.h"

/**
 * Cube
 * ----
 */
Cube::Cube() {
  // Add faces
  for (int f = 0; f < 6; f++) {
    int (&face)[4] = Cube::faces[f];

    // Add face corners
    for (int c = 0; c < 4; c++) {
      int cornerIndex = face[c];
      Vec3f position = Cube::corners[cornerIndex];
      Vec2f uv = Cube::uvs[c];

      addVertex(position, uv);
    }
  }

  // Add polygons
  for (int f = 0; f < 6; f++) {
    int offset = f * 4;

    addPolygon(offset, offset + 1, offset + 2);
    addPolygon(offset, offset + 2, offset + 3);
  }

  updateNormals();
}

// Unit cube corners as 3-vectors.
Vec3f Cube::corners[8] = {
  { -1.0f, -1.0f, -1.0f },
  { 1.0f, -1.0f, -1.0f },
  { 1.0f, 1.0f, -1.0f },
  { -1.0f, 1.0f, -1.0f },
  { -1.0f, -1.0f, 1.0f },
  { 1.0f, -1.0f, 1.0f },
  { 1.0f, 1.0f, 1.0f },
  { -1.0f, 1.0f, 1.0f }
};

// UV coordinates for each cube face.
Vec2f Cube::uvs[4] = {
  { 1.0f, 1.0f }, // Bottom right
  { 0.0f, 1.0f }, // Bottom left
  { 0.0f, 0.0f }, // Top left
  { 1.0f, 0.0f }  // Top right
};

// The six cube faces, each defined by
// four corner indices.
int Cube::faces[6][4] = {
  { 1, 0, 3, 2 }, // Back
  { 7, 6, 2, 3 }, // Top
  { 4, 5, 6, 7 }, // Front
  { 0, 1, 5, 4 }, // Bottom
  { 0, 4, 7, 3 }, // Left
  { 5, 1, 2, 6 }  // Right
};