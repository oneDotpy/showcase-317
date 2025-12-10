#include "blinn_phong_shading.h"
// Hint:
#include "first_hit.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Eigen::Vector3d blinn_phong_shading(
  const Ray & ray,
  const int & hit_id, 
  const double & t,
  const Eigen::Vector3d & n,
  const std::vector< std::shared_ptr<Object> > & objects,
  const std::vector<std::shared_ptr<Light> > & lights)
{
  const double EPS = 1e-8;

  // 1) Intersection point p and view vector v (pointing from p toward camera)
  const Eigen::Vector3d p = ray.origin + t * ray.direction;
  const Eigen::Vector3d v = (-ray.direction).normalized();

  // Access material (material is a shared_ptr in this starter)
  const Material &mat = *objects[hit_id]->material;

  // 2) Ambient once (ia = 0.1)
  const double ia = 0.1;
  Eigen::Vector3d L = ia * mat.ka;

  // 3) For each light: shadow test, then add diffuse + specular
  for (const auto &light : lights)
  {
    // Direction from p toward light, and how far to test (in parametric t)
    Eigen::Vector3d toL;
    double max_t;
    light->direction(p, toL, max_t);

    // Shadow ray
    Ray sray;
    sray.origin    = p + EPS * n;    
    sray.direction = toL.normalized();     // unit light direction

    // If something blocks before reaching the light, skip this light
    int sid; double st; Eigen::Vector3d sn;
    const bool occluded =
      first_hit(sray, EPS, objects, sid, st, sn) && (st < max_t);
    if (occluded) continue;

    // Light color/intensity
    const Eigen::Vector3d I = light->I;

    // Diffuse: kd * I * max(0, n·l)
    const double ndotl = std::max(0.0, n.dot(sray.direction));
    if (ndotl > 0.0) {
      const Eigen::Vector3d diffuse =
        (mat.kd.array() * I.array()).matrix() * ndotl;

      // Specular (Blinn-Phong): ks * I * max(0, n·h)^p
      const Eigen::Vector3d h = (sray.direction + v).normalized();
      const double ndoth = std::max(0.0, n.dot(h));
      const Eigen::Vector3d specular =
        (mat.ks.array() * I.array()).matrix() * std::pow(ndoth, mat.phong_exponent);

      L += diffuse + specular;
    }
  }

  return L;
}
