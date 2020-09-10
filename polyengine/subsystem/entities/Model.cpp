#include "subsystem/entities/Model.h"

/**
 * Model
 * -----
 */
void Model::from(const ObjLoader& loader) {
  bool hasTextureData = loader.textureCoordinates.size() > 0;

  if (hasTextureData) {
    buildTexturedModel(loader);
  } else {
    buildUntexturedModel(loader);
  }

  updateNormals();
}

void Model::from(Model* reference) {
  setReference(reference);
}

void Model::buildTexturedModel(const ObjLoader& loader) {
  // Since there may be a different number of defined vertex
  // vectors and vertex texture coordinates (owing to the way
  // .obj files store vertex information), we have to examine
  // the vertex index + texture coordinate index tuples defined
  // for each face, map these to a vertex vector and texture
  // coordinate, create and add a single vertex per unique tuple,
  // and finally add our polygons once all vertices are set.

  // Track unique vertex/texture coordinate index pairs and their
  // associated Object vertex index
  std::map<std::pair<int, int>, int> uniqueVertexIndexMap;

  // Track the vertex indices for each polygon, so we can create
  // them after all vertices are defined
  std::vector<std::tuple<int, int, int>> polygonVertexIndices;

  for (const Face& face : loader.faces) {
    std::pair<int, int> v_vt_pairs[3];
    int vertexIndices[3];

    v_vt_pairs[0] = { face.v1.vertexIndex, face.v1.textureCoordinateIndex };
    v_vt_pairs[1] = { face.v2.vertexIndex, face.v2.textureCoordinateIndex };
    v_vt_pairs[2] = { face.v3.vertexIndex, face.v3.textureCoordinateIndex };

    for (int t = 0; t < 3; t++) {
      std::pair<int, int>& pair = v_vt_pairs[t];
      auto uniqueVertex = uniqueVertexIndexMap.find(pair);

      if (uniqueVertex != uniqueVertexIndexMap.end()) {
        vertexIndices[t] = uniqueVertex->second;
      } else {
        const Vec3f& vector = loader.vertices.at(v_vt_pairs[t].first);
        Vec2f uv = loader.textureCoordinates.at(v_vt_pairs[t].second);
        int index = vertices.size();

        vertexIndices[t] = index;
        uv.y = 1 - uv.y;

        addVertex(vector, uv);

        uniqueVertexIndexMap.emplace(pair, index);
      }
    }

    polygonVertexIndices.push_back({ vertexIndices[0], vertexIndices[1], vertexIndices[2] });
  }

  for (const auto& vertexIndices : polygonVertexIndices) {
    addPolygon(std::get<0>(vertexIndices), std::get<1>(vertexIndices), std::get<2>(vertexIndices));
  }

  uniqueVertexIndexMap.clear();
  polygonVertexIndices.clear();
}

void Model::buildUntexturedModel(const ObjLoader& loader) {
  for (int v = 0; v < loader.vertices.size(); v++) {
    Vec3f vector = loader.vertices.at(v);

    addVertex(vector, Vec3f(1.0));
  }

  for (int f = 0; f < loader.faces.size(); f++) {
    const Face& face = loader.faces.at(f);

    int v1 = face.v1.vertexIndex;
    int v2 = face.v2.vertexIndex;
    int v3 = face.v3.vertexIndex;

    addPolygon(v1, v2, v3);
  }
}