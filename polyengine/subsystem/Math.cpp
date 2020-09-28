#include <cmath>
#include <cstdio>
#include <algorithm>

#include "subsystem/Math.h"

/**
 * Vec2f
 * -----
 */
const float* Vec2f::float2() const {
  return &x;
}

/**
 * Vec3f
 * -----
 */
Vec3f Vec3f::operator+(const Vec3f& vector) const {
  return {
    x + vector.x,
    y + vector.y,
    z + vector.z
  };
}

void Vec3f::operator+=(const Vec3f& vector) {
  x += vector.x;
  y += vector.y;
  z += vector.z;
}

Vec3f Vec3f::operator-(const Vec3f& vector) const {
  return {
    x - vector.x,
    y - vector.y,
    z - vector.z
  };
}

void Vec3f::operator-=(const Vec3f& vector) {
  x -= vector.x;
  y -= vector.y;
  z -= vector.z;
}

Vec3f Vec3f::operator*(float scalar) const {
  return {
    x * scalar,
    y * scalar,
    z * scalar
  };
}

Vec3f Vec3f::operator*(const Vec3f& vector) const {
  return {
    x * vector.x,
    y * vector.y,
    z * vector.z
  };
}

void Vec3f::operator*=(float scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
}

Vec3f Vec3f::operator/(float scalar) const {
  return {
    x / scalar,
    y / scalar,
    z / scalar
  };
}

Vec3f Vec3f::crossProduct(const Vec3f& v1, const Vec3f& v2) {
  return {
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x
  };
}

void Vec3f::debug() const {
  printf("[ %f ] [ %f ] [ %f ]\n", x, y, z);
}

const float* Vec3f::float3() const {
  return &x;
}

Vec3f Vec3f::gl() const {
  return *this * Vec3f(1.0f, 1.0f, -1.0f);
}

Vec3f Vec3f::invert() const {
  return *this * -1.0f;
}

float Vec3f::magnitude() const {
  return sqrtf(x*x + y*y + z*z);
}

Vec3f Vec3f::unit() const {
  float m = magnitude();

  return {
    x / m,
    y / m,
    z / m
  };
}

Vec3f Vec3f::xz() const {
  return *this * Vec3f(1.0f, 0.0f, 1.0f);
}

/**
 * Frustum
 * -------
 */
Bounds3d Frustum::getBounds() const {
  Bounds3d bounds;

  bounds.top = std::max({
    near[0].y, near[1].y, near[2].y, near[3].y,
    far[0].y, far[1].y, far[2].y, far[3].y
  });

  bounds.bottom = std::min({
    near[0].y, near[1].y, near[2].y, near[3].y,
    far[0].y, far[1].y, far[2].y, far[3].y
  });

  bounds.left = std::min({
    near[0].x, near[1].x, near[2].x, near[3].x,
    far[0].x, far[1].x, far[2].x, far[3].x
  });

  bounds.right = std::max({
    near[0].x, near[1].x, near[2].x, near[3].x,
    far[0].x, far[1].x, far[2].x, far[3].x
  });

  bounds.front = std::max({
    near[0].z, near[1].z, near[2].z, near[3].z,
    far[0].z, far[1].z, far[2].z, far[3].z
  });

  bounds.back = std::min({
    near[0].z, near[1].z, near[2].z, near[3].z,
    far[0].z, far[1].z, far[2].z, far[3].z
  });

  return bounds;
}

/**
 * Matrix4
 * -------
 */
void Matrix4::debug() const {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("[ %f ] ", m[i * 4 + j]);
    }

    printf("\n");
  }
}

Matrix4 Matrix4::fromMatrix3(const Matrix3& matrix) {
  return {
    matrix.m11, matrix.m12, matrix.m13, 0.0f,
    matrix.m21, matrix.m22, matrix.m23, 0.0f,
    matrix.m31, matrix.m32, matrix.m33, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
}

Matrix4 Matrix4::identity() {
  return {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
}

Matrix4 Matrix4::lookAt(const Vec3f& eye, const Vec3f& direction, const Vec3f& top) {
  Vec3f forward = direction.unit();
  Vec3f right = Vec3f::crossProduct(top, forward).unit();
  Vec3f up = Vec3f::crossProduct(forward, right).unit();
  Matrix4 translation = Matrix4::translate(eye.invert());

  Matrix4 rotation = {
    right.x, right.y, right.z, 0.0f,
    up.x, up.y, up.z, 0.0f,
    forward.x, forward.y, forward.z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  return rotation * translation;
}

Matrix4 Matrix4::orthographic(float top, float bottom, float left, float right, float near, float far) {
  return {
    2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
    0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
    0.0f, 0.0f, -2.0f / (far - near), -(far + near) / (far - near),
    0.0f, 0.0f, 0.0f, 1.0f
  };
}

Matrix4 Matrix4::projection(const Region2d<int>& area, float fov, float near, float far) {
  constexpr float PI = 3.14159265359;
  constexpr float DEG_TO_RAD = PI / 180.0f;
  float f = 1.0f / tanf(fov / 2.0f * DEG_TO_RAD);
  float aspectRatio = (float)area.width / (float)area.height;

  return {
    f / aspectRatio, 0.0f, 0.0f, 0.0f,
    0.0f, f, 0.0f, 0.0f,
    0.0f, 0.0f, (far + near) / (near - far), (2 * far * near) / (near - far),
    0.0f, 0.0f, -1.0f, 0.0f
  };
}

Matrix4 Matrix4::rotate(const Vec3f& rotation) {
  Quaternion pitch = Quaternion::fromAxisAngle(rotation.x, 1.0f, 0.0f, 0.0f);
  Quaternion yaw = Quaternion::fromAxisAngle(rotation.y, 0.0f, 1.0f, 0.0f);
  Quaternion roll = Quaternion::fromAxisAngle(rotation.z, 0.0f, 0.0f, 1.0f);

  return Matrix4::fromMatrix3((pitch * yaw * roll).toMatrix3());
}

Matrix4 Matrix4::scale(const Vec3f& scale) {
  return {
    scale.x, 0.0f, 0.0f, 0.0f,
    0.0f, scale.y, 0.0f, 0.0f,
    0.0f, 0.0f, scale.z, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
}

Matrix4 Matrix4::translate(const Vec3f& translation) {
  return {
    1.0f, 0.0f, 0.0f, translation.x,
    0.0f, 1.0f, 0.0f, translation.y,
    0.0f, 0.0f, 1.0f, translation.z,
    0.0f, 0.0f, 0.0f, 1.0f
  };
}

Matrix4 Matrix4::transpose() const {
  return {
    m[0], m[4], m[8], m[12],
    m[1], m[5], m[9], m[13],
    m[2], m[6], m[10], m[14],
    m[3], m[7], m[11], m[15]
  };
}

Matrix4 Matrix4::operator*(const Matrix4& matrix) const {
  Matrix4 product;

  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      float& value = product.m[r * 4 + c] = 0;

      for (int n = 0; n < 4; n++) {
        value += m[r * 4 + n] * matrix.m[n * 4 + c];
      }
    }
  }

  return product;
}

Vec3f Matrix4::operator*(const Vec3f& vector) const {
  float x = vector.x;
  float y = vector.y;
  float z = vector.z;
  float w = 1.0f;

  return Vec3f(
    x * m[0] + y * m[1] + z * m[2] + w * m[3],
    x * m[4] + y * m[5] + z * m[6] + w * m[7],
    x * m[8] + y * m[9] + z * m[10] + w * m[11]
  );
}

Frustum Matrix4::operator*(const Frustum& frustum) const {
  Frustum product;

  product.near[0] = *this * frustum.near[0];
  product.near[1] = *this * frustum.near[1];
  product.near[2] = *this * frustum.near[2];
  product.near[3] = *this * frustum.near[3];

  product.far[0] = *this * frustum.far[0];
  product.far[1] = *this * frustum.far[1];
  product.far[2] = *this * frustum.far[2];
  product.far[3] = *this * frustum.far[3];

  return product;
}

/**
 * Quaternion
 * ----------
 */
Quaternion Quaternion::fromAxisAngle(float angle, float x, float y, float z) {
  float sa = sinf(angle / 2);

  return {
    cosf(angle / 2),
    x * sa,
    y * sa,
    z * sa
  };
}

Matrix3 Quaternion::toMatrix3() const {
  return {
    1 - 2 * y * y - 2 * z * z, 2 * x * y - 2 * z * w, 2 * x * z + 2 * y * w,
    2 * x * y + 2 * z * w, 1 - 2 * x * x - 2 * z * z, 2 * y * z - 2 * x * w,
    2 * x * z - 2 * y * w, 2 * y * z + 2 * x * w, 1 - 2 * x * x - 2 * y * y
  };
}

Quaternion Quaternion::operator*(const Quaternion& q2) const {
  return {
    w * q2.w - x * q2.x - y * q2.y - z * q2.z,
    w * q2.x + x * q2.w + y * q2.z - z * q2.y,
    w * q2.y - x * q2.z + y * q2.w + z * q2.x,
    w * q2.z + x * q2.y - y * q2.x + z * q2.w
  };
}