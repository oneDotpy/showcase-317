#include "Sphere.h"
#include "Ray.h"

bool Sphere::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  // Sphere equation: |(o + t d) - c|^2 = r^2
  // Solve quadratic in t: (d·d)t^2 + 2oc·d t + (oc·oc - r^2) = 0

  Eigen::Vector3d oc = ray.origin - center;
  const Eigen::Vector3d & d = ray.direction;

  double a = d.dot(d);                     // usually 1 if d is normalized
  double b = 2.0 * oc.dot(d);
  double c = oc.dot(oc) - radius * radius;

  double discriminant = b * b - 4.0 * a * c;
  if (discriminant < 0.0) {
    return false; // no real roots = no intersection
  }

  double sqrt_disc = std::sqrt(discriminant);
  double t0 = (-b - sqrt_disc) / (2.0 * a);
  double t1 = (-b + sqrt_disc) / (2.0 * a);

  // Pick the closest valid t >= min_t
  const double eps = 1e-9;
  double tt = std::numeric_limits<double>::infinity();
  if (t0 >= min_t + eps) {
    tt = t0;
  } else if (t1 >= min_t + eps) {
    tt = t1;
  } else {
    return false; // both behind the ray or too close
  }

  t = tt;
  Eigen::Vector3d p = ray.origin + t * d;
  n = (p - center).normalized();
  return true;
}

