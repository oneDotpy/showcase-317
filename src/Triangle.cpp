#include "Triangle.h"
#include "Ray.h"
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <iostream>

bool Triangle::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  const Eigen::Vector3d & v0 = std::get<0>(corners);
  const Eigen::Vector3d & v1 = std::get<1>(corners);
  const Eigen::Vector3d & v2 = std::get<2>(corners);

  const Eigen::Vector3d e1 = v1 - v0;
  const Eigen::Vector3d e2 = v2 - v0;

  const Eigen::Vector3d pvec = ray.direction.cross(e2);
  const double det = e1.dot(pvec);

  const double eps = 1e-9;
  if (std::abs(det) < eps) return false; // ray parallel to triangle

  const double invDet = 1.0 / det;

  const Eigen::Vector3d tvec = ray.origin - v0;
  const double u = tvec.dot(pvec) * invDet;
  if (u < 0.0 || u > 1.0) return false;

  const Eigen::Vector3d qvec = tvec.cross(e1);
  const double v = ray.direction.dot(qvec) * invDet;
  if (v < 0.0 || u + v > 1.0) return false;

  const double tt = e2.dot(qvec) * invDet;
  if (tt < min_t + eps) return false;

  t = tt;
  n = e1.cross(e2).normalized();
  return true;
}


