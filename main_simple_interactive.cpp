// Simple interactive parameter adjuster for Film Camera Dreams
// Renders to file on parameter changes

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstdlib>

#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "read_json.h"
#include "raycolor.h"
#include "viewing_ray_dof.h"
#include "viewing_ray.h"
#include "post_process.h"
#include "write_ppm.h"

struct RenderParams {
  double aperture = 0.12;
  double focal_distance = 3.3;
  bool enable_grain = true;
  bool enable_vignette = true;
  bool enable_grading = true;
  int samples_per_pixel = 8;  // Lower for faster iteration
};

void print_menu(const RenderParams& params) {
  system("cls");  // Windows clear screen
  std::cout << "\n========================================" << std::endl;
  std::cout << "  Film Camera Dreams - Parameter Tuner" << std::endl;
  std::cout << "========================================\n" << std::endl;
  std::cout << "Current Settings:" << std::endl;
  std::cout << "  Aperture:        " << params.aperture << std::endl;
  std::cout << "  Focal Distance:  " << params.focal_distance << std::endl;
  std::cout << "  Film Grain:      " << (params.enable_grain ? "ON" : "OFF") << std::endl;
  std::cout << "  Vignetting:      " << (params.enable_vignette ? "ON" : "OFF") << std::endl;
  std::cout << "  Color Grading:   " << (params.enable_grading ? "ON" : "OFF") << std::endl;
  std::cout << "  Samples/Pixel:   " << params.samples_per_pixel << std::endl;
  std::cout << "\nControls:" << std::endl;
  std::cout << "  1 - Increase aperture" << std::endl;
  std::cout << "  2 - Decrease aperture" << std::endl;
  std::cout << "  3 - Increase focal distance" << std::endl;
  std::cout << "  4 - Decrease focal distance" << std::endl;
  std::cout << "  g - Toggle film grain" << std::endl;
  std::cout << "  v - Toggle vignetting" << std::endl;
  std::cout << "  c - Toggle color grading" << std::endl;
  std::cout << "  + - Increase samples (higher quality)" << std::endl;
  std::cout << "  - - Decrease samples (faster render)" << std::endl;
  std::cout << "  r - Render with current settings" << std::endl;
  std::cout << "  q - Quit\n" << std::endl;
}

int main(int argc, char* argv[]) {
  std::string scene_file = "data/showcase.json";
  if (argc > 1) {
    scene_file = argv[1];
  }

  // Load scene
  Camera camera;
  std::vector<std::shared_ptr<Object>> objects;
  std::vector<std::shared_ptr<Light>> lights;
  read_json(scene_file, camera, objects, lights);

  RenderParams params;
  params.aperture = camera.aperture;
  params.focal_distance = camera.focal_distance;

  int width = 640;
  int height = 360;

  while (true) {
    print_menu(params);

    char choice;
    std::cin >> choice;

    bool should_render = false;

    switch (choice) {
      case '1':
        params.aperture += 0.02;
        should_render = true;
        break;
      case '2':
        params.aperture = std::max(0.0, params.aperture - 0.02);
        should_render = true;
        break;
      case '3':
        params.focal_distance += 0.2;
        should_render = true;
        break;
      case '4':
        params.focal_distance = std::max(0.1, params.focal_distance - 0.2);
        should_render = true;
        break;
      case 'g':
      case 'G':
        params.enable_grain = !params.enable_grain;
        should_render = true;
        break;
      case 'v':
      case 'V':
        params.enable_vignette = !params.enable_vignette;
        should_render = true;
        break;
      case 'c':
      case 'C':
        params.enable_grading = !params.enable_grading;
        should_render = true;
        break;
      case '+':
      case '=':
        params.samples_per_pixel = std::min(128, params.samples_per_pixel * 2);
        should_render = true;
        break;
      case '-':
      case '_':
        params.samples_per_pixel = std::max(1, params.samples_per_pixel / 2);
        should_render = true;
        break;
      case 'r':
      case 'R':
        should_render = true;
        break;
      case 'q':
      case 'Q':
        std::cout << "\nExiting..." << std::endl;
        return 0;
      default:
        continue;
    }

    if (should_render) {
      std::cout << "\nRendering..." << std::flush;

      camera.aperture = params.aperture;
      camera.focal_distance = params.focal_distance;

      std::vector<uint8_t> rgb_image(width * height * 3, 0);

      for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
          Eigen::Vector3d color(0, 0, 0);

          for (int s = 0; s < params.samples_per_pixel; ++s) {
            double u = (i + ((double)rand() / RAND_MAX)) / width;
            double v = (j + ((double)rand() / RAND_MAX)) / height;

            Ray ray;
            if (camera.aperture > 0.0) {
              double lens_u = (double)rand() / RAND_MAX;
              double lens_v = (double)rand() / RAND_MAX;
              viewing_ray_dof(camera, i, j, width, height, u, v, lens_u, lens_v, ray);
            } else {
              viewing_ray(camera, i, j, width, height, ray);
            }

            color += raycolor(ray, 5, objects, lights);
          }

          color /= params.samples_per_pixel;

          // Post-processing
          if (params.enable_grading) {
            color = apply_warm_grading(color, 0.25);
          }
          if (params.enable_vignette) {
            color = apply_vignetting(color, i, j, width, height, 0.45);
          }
          if (params.enable_grain) {
            color = apply_film_grain(color, i, j, 0.018);
          }

          int idx = ((height - 1 - j) * width + i) * 3;
          rgb_image[idx + 0] = std::min(255.0, std::max(0.0, color(0) * 255.0));
          rgb_image[idx + 1] = std::min(255.0, std::max(0.0, color(1) * 255.0));
          rgb_image[idx + 2] = std::min(255.0, std::max(0.0, color(2) * 255.0));
        }
      }

      write_ppm("preview.ppm", rgb_image, width, height);
      std::cout << " Done! Saved to preview.ppm" << std::endl;
      std::cout << "Press Enter to continue...";
      std::cin.ignore();
      std::cin.get();

      // Try to open the image
      system("preview.ppm");
    }
  }

  return 0;
}
