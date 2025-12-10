#include "first_hit.h"

bool first_hit(
  const Ray & ray, 
  const double min_t,
  const std::vector< std::shared_ptr<Object> > & objects,
  int & hit_id, 
  double & t,
  Eigen::Vector3d & n)
{
  bool hit = false;
  double best_t = std::numeric_limits<double>::infinity();
  Eigen::Vector3d best_n(0,0,0);
  int best_id = -1;

  for (int k = 0; k < static_cast<int>(objects.size()); ++k) {
    double tk;
    Eigen::Vector3d nk;
    if (objects[k]->intersect(ray, min_t, tk, nk)) {
      if (tk < best_t) {
        best_t = tk;
        best_n = nk;
        best_id = k;
        hit = true;
      }
    }
  }

  if (!hit) return false;
  t = best_t;
  n = best_n;
  hit_id = best_id;
  return true;
}
