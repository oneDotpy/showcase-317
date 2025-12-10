#define _USE_MATH_DEFINES
#include <cmath>
#include "random_disk_sample.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Eigen::Vector2d random_disk_sample(double u, double v)
{
  // Concentric disk sampling for better distribution
  // Ref: "A Low Distortion Map Between Disk and Square" by Shirley & Chiu
  // https://psgraphics.blogspot.com/2011/01/improved-code-for-concentric-map.html

  // Map [0,1] to [-1,1]
  double a = 2.0 * u - 1.0;
  double b = 2.0 * v - 1.0;

  if (a == 0.0 && b == 0.0) {
    return Eigen::Vector2d(0, 0);
  }

  double r, theta;
  if (a * a > b * b) {
    r = a;
    theta = (M_PI / 4.0) * (b / a);
  } else {
    r = b;
    theta = (M_PI / 2.0) - (M_PI / 4.0) * (a / b);
  }

  return Eigen::Vector2d(r * cos(theta), r * sin(theta));
}
