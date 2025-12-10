#include "raycolor.h"
#include "first_hit.h"
#include "blinn_phong_shading.h"
#include "reflect.h"

bool raycolor(
  const Ray & ray, 
  const double min_t,
  const std::vector< std::shared_ptr<Object> > & objects,
  const std::vector< std::shared_ptr<Light> > & lights,
  const int num_recursive_calls,
  Eigen::Vector3d & rgb)
{
   const double EPS = 1e-6;
  rgb.setZero();

  // 1) Find first intersection
  int hit_id; double t; Eigen::Vector3d n;
  if(!first_hit(ray, min_t, objects, hit_id, t, n))
  {
    // no hit → background (black)
    return false;
  }

  // 2) Local shading (ambient + diffuse + specular + shadows)
  rgb = blinn_phong_shading(ray, hit_id, t, n, objects, lights);

  // 3) Recursive mirror reflection (depth limit; km is mirror coefficient)
  const Material &mat = *objects[hit_id]->material;

  // set a reasonable max recursion depth
  const int MAX_DEPTH = 9;
  if(num_recursive_calls < MAX_DEPTH && mat.km.maxCoeff() > 0.0)
  {
    // construct mirror ray
    const Eigen::Vector3d p = ray.origin + t * ray.direction;
    Ray mirror_ray;
    mirror_ray.origin    = p + EPS * n;                 
    mirror_ray.direction = reflect(ray.direction, n);

    Eigen::Vector3d rec_rgb(0,0,0);
    // recurse
    raycolor(mirror_ray, EPS, objects, lights, num_recursive_calls + 1, rec_rgb);

    // accumulate with mirror coefficient (component-wise)
    rgb += mat.km.cwiseProduct(rec_rgb);
  }

  return true;
}
