#include "write_ppm.h"
#include <fstream>
#include <cassert>
#include <iostream>


bool write_ppm(
  const std::string & filename,
  const std::vector<unsigned char> & data,
  const int width,
  const int height,
  const int num_channels)
{
  assert(
    (num_channels == 3 || num_channels ==1 ) &&
    ".ppm only supports RGB or grayscale images");
    
    // Opens the file
    std::ofstream out(filename);
    if (!out) return false;

    // Make Header
    out << (num_channels == 3 ? "P3\n" : "P2\n");
    out << width << " " << height << "\n";
    out << 255 << "\n";

    // Generate each pixels data
    std::size_t idx = 0;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        for (int c = 0; c < num_channels; ++c) {
          out << static_cast<int>(data[idx++]) << " ";
        }
      }
      out << "\n";
    }

    return true;
}
