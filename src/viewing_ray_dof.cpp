#include "viewing_ray_dof.h"
#include "random_disk_sample.h"

void viewing_ray_dof(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  const double u,
  const double v,
  Ray & ray)
{
  // Pixel center offsets in scene units on the image plane
  const double sx = ( (j + 0.5) / static_cast<double>(width)  - 0.5 ) * camera.width;
  const double sy = -( (i + 0.5) / static_cast<double>(height) - 0.5 ) * camera.height;

  // Point on the image plane (pinhole camera)
  const Eigen::Vector3d p_image =
      camera.e
    - camera.d * camera.w
    + sx * camera.u
    + sy * camera.v;

  // Direction from pinhole to image plane point
  Eigen::Vector3d ray_dir = (p_image - camera.e).normalized();

  // Find focal point: where this ray intersects the focal plane
  // The focal plane is perpendicular to -w at distance focal_distance from camera.e
  // We need to find where ray_dir intersects this plane
  // Plane equation: dot(point - focal_center, -camera.w) = 0
  // where focal_center = camera.e - focal_distance * camera.w
  Eigen::Vector3d focal_center = camera.e - camera.focal_distance * camera.w;
  double denom = ray_dir.dot(-camera.w);
  double t_focal = (focal_center - camera.e).dot(-camera.w) / denom;
  Eigen::Vector3d focal_point = camera.e + t_focal * ray_dir;

  // If aperture is 0, this is a pinhole camera (no DOF)
  if (camera.aperture <= 0.0) {
    ray.origin = camera.e;
    ray.direction = ray_dir;
    return;
  }

  // Sample a random point on the lens (thin lens approximation)
  Eigen::Vector2d disk_sample = random_disk_sample(u, v);
  Eigen::Vector3d lens_point = camera.e
    + camera.aperture * disk_sample(0) * camera.u
    + camera.aperture * disk_sample(1) * camera.v;

  // Ray from lens point through focal point
  ray.origin = lens_point;
  ray.direction = (focal_point - lens_point).normalized();
}
