#include "viewing_ray.h"

void viewing_ray(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  Ray & ray)
{
  // Pixel center offsets in scene units on the image plane
  const double sx = ( (j + 0.5) / static_cast<double>(width)  - 0.5 ) * camera.width;
  const double sy = -( (i + 0.5) / static_cast<double>(height) - 0.5 ) * camera.height; // top-left origin = minus

  // Point on the image plane
  const Eigen::Vector3d p =
      camera.e
    - camera.d * camera.w   // image plane center in front of the eye
    + sx * camera.u
    + sy * camera.v;

  ray.origin    = camera.e;
  ray.direction = (p - camera.e);
}
