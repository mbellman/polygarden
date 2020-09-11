layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexUv;
layout (location = 4) in int objectId;
layout (location = 5) in vec3 modelColor;
layout (location = 6) in mat4 modelMatrix;

struct VertexAttributes {
  vec3 position;
  vec3 normal;
  vec3 tangent;
  vec2 uv;
};

struct InstanceAttributes {
  int ID;
  vec3 color;
  mat4 matrix;
};

VertexAttributes Vertex = VertexAttributes(vertexPosition, vertexNormal, vertexTangent, vertexUv);
InstanceAttributes Instance = InstanceAttributes(objectId, modelColor, modelMatrix);