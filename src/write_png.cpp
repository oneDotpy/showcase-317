#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "write_png.h"

bool write_png(
  const std::string & filename,
  const std::vector<uint8_t> & data,
  const int width,
  const int height)
{
  // stbi_write_png expects data in row-major order from top to bottom
  // Our data is already in the correct format (flipped during rendering)
  int result = stbi_write_png(
    filename.c_str(),
    width,
    height,
    3,  // RGB = 3 components
    data.data(),
    width * 3  // stride in bytes
  );

  return result != 0;
}
