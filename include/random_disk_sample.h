#ifndef RANDOM_DISK_SAMPLE_H
#define RANDOM_DISK_SAMPLE_H

#include <Eigen/Core>

// Sample a random point uniformly within a unit disk
// Uses the concentric mapping method for better distribution
//
// Inputs:
//   u, v  random numbers in [0,1]
// Returns:
//   2D point in unit disk (x,y) where x^2 + y^2 <= 1
Eigen::Vector2d random_disk_sample(double u, double v);

#endif
