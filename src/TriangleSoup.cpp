#include "TriangleSoup.h"
#include "Triangle.h"
#include "first_hit.h"
bool TriangleSoup::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  bool hit = false;
  double best_t = std::numeric_limits<double>::infinity();
  Eigen::Vector3d best_n(0,0,0);
  for (const auto & tri : triangles) {
    double tk;
    Eigen::Vector3d nk;
    if (tri->intersect(ray, min_t, tk, nk)) {
      if (tk < best_t) {
        best_t = tk;
        best_n = nk;
        hit = true;
      }
    }
  }

  if (!hit) return false;
  t = best_t;
  n = best_n;
  return true;
}
