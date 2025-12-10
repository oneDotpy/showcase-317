#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <Eigen/Core>

// Apply film grain effect to a color
// Adds subtle random noise to simulate film grain
//
// Inputs:
//   color  input RGB color
//   i, j   pixel coordinates (for seeding random)
//   intensity  grain strength (0.0 = none, 0.02 = subtle)
// Returns:
//   color with grain applied
Eigen::Vector3d apply_film_grain(
  const Eigen::Vector3d & color,
  int i, int j,
  double intensity = 0.015);

// Apply vignetting effect
// Darkens the corners and edges of the image like a camera lens
//
// Inputs:
//   color  input RGB color
//   i, j   pixel coordinates
//   width, height  image dimensions
//   strength  vignetting strength (0.0 = none, 0.5 = moderate, 1.0 = strong)
// Returns:
//   color with vignetting applied
Eigen::Vector3d apply_vignetting(
  const Eigen::Vector3d & color,
  int i, int j,
  int width, int height,
  double strength = 0.4);

// Apply warm color grading for film aesthetic
// Shifts colors to warmer tones (more red/yellow)
//
// Inputs:
//   color  input RGB color
//   strength  grading strength (0.0 = none, 0.3 = subtle, 1.0 = strong)
// Returns:
//   color graded color
Eigen::Vector3d apply_warm_grading(
  const Eigen::Vector3d & color,
  double strength = 0.2);

#endif
