/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace Math
{
  constexpr float PI = 3.14159265359f;

  float sinf(float angle);

  static float cosf(float angle) {
    return sinf(angle + (PI / 2.0f));
  }
}

// Operator overloads
[[maybe_unused]] inline fm_vec3_t operator+(fm_vec3_t const& lhs, fm_vec3_t const& rhs) {
  return {lhs.v[0] + rhs.v[0], lhs.v[1] + rhs.v[1], lhs.v[2] + rhs.v[2]};
}

[[maybe_unused]] inline fm_vec3_t operator-(fm_vec3_t const& lhs, fm_vec3_t const& rhs) {
  return {lhs.v[0] - rhs.v[0], lhs.v[1] - rhs.v[1], lhs.v[2] - rhs.v[2]};
}

[[maybe_unused]] inline fm_vec3_t operator*(fm_vec3_t const& lhs, fm_vec3_t const& rhs) {
  return {lhs.v[0] * rhs.v[0], lhs.v[1] * rhs.v[1], lhs.v[2] * rhs.v[2]};
}

[[maybe_unused]] inline fm_vec3_t operator/(fm_vec3_t const& lhs, fm_vec3_t const& rhs) {
  return {lhs.v[0] / rhs.v[0], lhs.v[1] / rhs.v[1], lhs.v[2] / rhs.v[2]};
}

[[maybe_unused]] inline fm_vec3_t operator+(fm_vec3_t const& lhs, float rhs) {
  return {lhs.v[0] + rhs, lhs.v[1] + rhs, lhs.v[2] + rhs};
}

[[maybe_unused]] inline fm_vec3_t operator-(fm_vec3_t const& lhs, float rhs) {
  return {lhs.v[0] - rhs, lhs.v[1] - rhs, lhs.v[2] - rhs};
}

[[maybe_unused]] inline fm_vec3_t operator*(fm_vec3_t const& lhs, float rhs) {
  return {lhs.v[0] * rhs, lhs.v[1] * rhs, lhs.v[2] * rhs};
}

[[maybe_unused]] inline fm_vec3_t operator/(fm_vec3_t const& lhs, float rhs) {
  return {lhs.v[0] / rhs, lhs.v[1] / rhs, lhs.v[2] / rhs};
}

[[maybe_unused]] inline fm_vec3_t operator-(fm_vec3_t const& lhs) {
  return {-lhs.v[0], -lhs.v[1], -lhs.v[2]};
}

[[maybe_unused]] inline fm_vec3_t& operator+=(fm_vec3_t &lhs, fm_vec3_t const& rhs) {
  lhs.v[0] += rhs.v[0];
  lhs.v[1] += rhs.v[1];
  lhs.v[2] += rhs.v[2];
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator+=(fm_vec3_t &lhs, float rhs) {
  lhs.v[0] += rhs;
  lhs.v[1] += rhs;
  lhs.v[2] += rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator-=(fm_vec3_t &lhs, fm_vec3_t const& rhs) {
  lhs.v[0] -= rhs.v[0];
  lhs.v[1] -= rhs.v[1];
  lhs.v[2] -= rhs.v[2];
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator-=(fm_vec3_t &lhs, float rhs) {
  lhs.v[0] -= rhs;
  lhs.v[1] -= rhs;
  lhs.v[2] -= rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator*=(fm_vec3_t &lhs, fm_vec3_t const& rhs) {
  lhs.v[0] *= rhs.v[0];
  lhs.v[1] *= rhs.v[1];
  lhs.v[2] *= rhs.v[2];
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator*=(fm_vec3_t &lhs, float rhs) {
  lhs.v[0] *= rhs;
  lhs.v[1] *= rhs;
  lhs.v[2] *= rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator/=(fm_vec3_t &lhs, fm_vec3_t const& rhs) {
  lhs.v[0] /= rhs.v[0];
  lhs.v[1] /= rhs.v[1];
  lhs.v[2] /= rhs.v[2];
  return lhs;
}

[[maybe_unused]] inline fm_vec3_t& operator/=(fm_vec3_t &lhs, float rhs) {
  lhs.v[0] /= rhs;
  lhs.v[1] /= rhs;
  lhs.v[2] /= rhs;
  return lhs;
}

[[maybe_unused]] inline fm_quat_t operator*(fm_quat_t const& lhs, fm_quat_t const& rhs) {
  fm_quat_t res;
  fm_quat_mul(&res, const_cast<fm_quat_t*>(&lhs), const_cast<fm_quat_t*>(&rhs));
  return res;
}

[[maybe_unused]] inline fm_vec2_t operator+(fm_vec2_t const& lhs, fm_vec2_t const& rhs) {
  return {lhs.v[0] + rhs.v[0], lhs.v[1] + rhs.v[1]};
}

[[maybe_unused]] inline fm_vec2_t operator-(fm_vec2_t const& lhs, fm_vec2_t const& rhs) {
  return {lhs.v[0] - rhs.v[0], lhs.v[1] - rhs.v[1]};
}

[[maybe_unused]] inline fm_vec2_t operator*(fm_vec2_t const& lhs, fm_vec2_t const& rhs) {
  return {lhs.v[0] * rhs.v[0], lhs.v[1] * rhs.v[1]};
}

[[maybe_unused]] inline fm_vec2_t operator/(fm_vec2_t const& lhs, fm_vec2_t const& rhs) {
  return {lhs.v[0] / rhs.v[0], lhs.v[1] / rhs.v[1]};
}

[[maybe_unused]] inline fm_vec2_t operator+(fm_vec2_t const& lhs, float rhs) {
  return {lhs.v[0] + rhs, lhs.v[1] + rhs};
}

[[maybe_unused]] inline fm_vec2_t operator-(fm_vec2_t const& lhs, float rhs) {
  return {lhs.v[0] - rhs, lhs.v[1] - rhs};
}

[[maybe_unused]] inline fm_vec2_t operator*(fm_vec2_t const& lhs, float rhs) {
  return {lhs.v[0] * rhs, lhs.v[1] * rhs};
}

[[maybe_unused]] inline fm_vec2_t operator/(fm_vec2_t const& lhs, float rhs) {
  return {lhs.v[0] / rhs, lhs.v[1] / rhs};
}

[[maybe_unused]] inline fm_vec2_t operator-(fm_vec2_t const& lhs) {
  return {-lhs.v[0], -lhs.v[1]};
}

[[maybe_unused]] inline fm_vec2_t& operator+=(fm_vec2_t &lhs, fm_vec2_t const& rhs) {
  lhs.v[0] += rhs.v[0];
  lhs.v[1] += rhs.v[1];
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator+=(fm_vec2_t &lhs, float rhs) {
  lhs.v[0] += rhs;
  lhs.v[1] += rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator-=(fm_vec2_t &lhs, fm_vec2_t const& rhs) {
  lhs.v[0] -= rhs.v[0];
  lhs.v[1] -= rhs.v[1];
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator-=(fm_vec2_t &lhs, float rhs) {
  lhs.v[0] -= rhs;
  lhs.v[1] -= rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator*=(fm_vec2_t &lhs, fm_vec2_t const& rhs) {
  lhs.v[0] *= rhs.v[0];
  lhs.v[1] *= rhs.v[1];
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator*=(fm_vec2_t &lhs, float rhs) {
  lhs.v[0] *= rhs;
  lhs.v[1] *= rhs;
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator/=(fm_vec2_t &lhs, fm_vec2_t const& rhs) {
  lhs.v[0] /= rhs.v[0];
  lhs.v[1] /= rhs.v[1];
  return lhs;
}

[[maybe_unused]] inline fm_vec2_t& operator/=(fm_vec2_t &lhs, float rhs) {
  lhs.v[0] /= rhs;
  lhs.v[1] /= rhs;
  return lhs;
}
