#include "DirectionalLight.h"
#include <limits>

void DirectionalLight::direction(
  const Eigen::Vector3d & q, Eigen::Vector3d & d, double & max_t) const
{
  (void) q;

  // Light direction stored is "from light to scene"
  // So "from point to light" is the opposite
  d = -this->d.normalized();

  // Directional lights are at infinity
  max_t = std::numeric_limits<double>::infinity();

  
}

