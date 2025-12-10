#ifndef WRITE_PNG_H
#define WRITE_PNG_H

#include <string>
#include <vector>
#include <cstdint>

// Write RGB image data to PNG file
//
// Inputs:
//   filename  path to output .png file
//   data      width*height*3 array of RGB values (0-255)
//   width     image width in pixels
//   height    image height in pixels
// Returns true on success
bool write_png(
  const std::string & filename,
  const std::vector<uint8_t> & data,
  const int width,
  const int height);

#endif
