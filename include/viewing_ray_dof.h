#ifndef VIEWING_RAY_DOF_H
#define VIEWING_RAY_DOF_H

#include "Ray.h"
#include "Camera.h"

// Construct a viewing ray with depth of field (thin lens model)
//
// Inputs:
//   camera  Perspective camera with aperture and focal distance
//   i  pixel row index
//   j  pixel column index
//   width  number of pixels width of image
//   height  number of pixels height of image
//   u, v  random numbers in [0,1] for lens sampling
// Outputs:
//   ray  viewing ray from random point on lens through focal point
void viewing_ray_dof(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  const double u,
  const double v,
  Ray & ray);

#endif
