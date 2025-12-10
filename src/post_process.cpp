#include "post_process.h"
#include <cmath>
#include <algorithm>

// Helper smoothstep function
static double smoothstep(double edge0, double edge1, double x) {
  x = std::max(0.0, std::min(1.0, (x - edge0) / (edge1 - edge0)));
  return x * x * (3.0 - 2.0 * x);
}

// Simple hash function for pseudo-random numbers from pixel coordinates
static double hash(int i, int j, int seed = 0) {
  int n = i * 374761393 + j * 668265263 + seed;
  n = (n ^ (n >> 13)) * 1274126177;
  n = n ^ (n >> 16);
  return (n & 0x7fffffff) / double(0x7fffffff);
}

Eigen::Vector3d apply_film_grain(
  const Eigen::Vector3d & color,
  int i, int j,
  double intensity)
{
  // Generate pseudo-random grain for each color channel
  double grain_r = (hash(i, j, 0) - 0.5) * 2.0 * intensity;
  double grain_g = (hash(i, j, 1) - 0.5) * 2.0 * intensity;
  double grain_b = (hash(i, j, 2) - 0.5) * 2.0 * intensity;

  Eigen::Vector3d result = color;
  result(0) += grain_r;
  result(1) += grain_g;
  result(2) += grain_b;

  // Clamp to valid range
  result(0) = std::max(0.0, std::min(1.0, result(0)));
  result(1) = std::max(0.0, std::min(1.0, result(1)));
  result(2) = std::max(0.0, std::min(1.0, result(2)));

  return result;
}

Eigen::Vector3d apply_vignetting(
  const Eigen::Vector3d & color,
  int i, int j,
  int width, int height,
  double strength)
{
  // Normalize coordinates to [-1, 1]
  double x = (j / double(width) - 0.5) * 2.0;
  double y = (i / double(height) - 0.5) * 2.0;

  // Distance from center (account for aspect ratio)
  double aspect = double(width) / double(height);
  double dist = sqrt(x * x * aspect * aspect + y * y);

  // Smooth falloff using smoothstep-like function
  double vignette = 1.0 - smoothstep(0.4, 1.4, dist) * strength;

  return color * vignette;
}

Eigen::Vector3d apply_warm_grading(
  const Eigen::Vector3d & color,
  double strength)
{
  // Warm grading: boost reds slightly, reduce blues
  // Similar to a warming photo filter or vintage film
  Eigen::Vector3d warm_tint(1.0 + strength * 0.15, 1.0 + strength * 0.05, 1.0 - strength * 0.1);

  Eigen::Vector3d result;
  result(0) = color(0) * warm_tint(0);
  result(1) = color(1) * warm_tint(1);
  result(2) = color(2) * warm_tint(2);

  // Optional: slight contrast boost for film look
  double contrast = 1.0 + strength * 0.1;
  result = (result - Eigen::Vector3d(0.5, 0.5, 0.5)) * contrast + Eigen::Vector3d(0.5, 0.5, 0.5);

  // Clamp
  result(0) = std::max(0.0, std::min(1.0, result(0)));
  result(1) = std::max(0.0, std::min(1.0, result(1)));
  result(2) = std::max(0.0, std::min(1.0, result(2)));

  return result;
}
