#include <Eigen/Core>

Eigen::Vector3d reflect(const Eigen::Vector3d & in, const Eigen::Vector3d & n)
{
  // Ensure the normal is unit length
  Eigen::Vector3d nn = n.normalized();

  // Mirror reflection formula from tutorial
  return in - 2.0 * (in.dot(nn)) * nn;
}
