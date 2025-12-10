#include "Plane.h"
#include "Ray.h"
#include <iostream>
bool Plane::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{// Equation: (p - point) · normal = 0
  // Ray: r(t) = ray.origin + t * ray.direction

  const double denom = normal.dot(ray.direction);
  const double eps = 1e-9;

  // If denom ~ 0, ray is parallel to the plane = no intersection
  if (std::abs(denom) < eps) {
    return false;
  }

  const double tt = normal.dot(point - ray.origin) / denom;

  // Only accept intersections in front of ray origin, and beyond min_t
  if (tt < min_t + eps) {
    return false;
  }

  t = tt;
  n = normal.normalized();
  return true;
}



