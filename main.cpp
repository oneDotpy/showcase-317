#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "read_json.h"
#include "write_ppm.h"
#include "write_png.h"
#include "viewing_ray.h"
#include "viewing_ray_dof.h"
#include "raycolor.h"
#include "post_process.h"
#include <Eigen/Core>
#include <vector>
#include <iostream>
#include <memory>
#include <limits>
#include <functional>
#include <random>


int main(int argc, char * argv[])
{
  Camera camera;
  std::vector< std::shared_ptr<Object> > objects;
  std::vector< std::shared_ptr<Light> > lights;
  // Read a camera and scene description from given .json file
  read_json(
    argc<=1?"../data/sphere-and-plane.json":argv[1],
    camera,
    objects,
    lights);

  // High quality render settings
  int width =  1280;  // High resolution for showcase
  int height = 720;
  int samples_per_pixel = 32;  // High quality samples for smooth bokeh

  std::cout << "Rendering " << width << "x" << height
            << " with " << samples_per_pixel << " samples/pixel..." << std::endl;

  // Random number generator for sampling
  std::mt19937 rng(42);  // Fixed seed for reproducibility
  std::uniform_real_distribution<double> uniform(0.0, 1.0);

  std::vector<unsigned char> rgb_image(3*width*height);

  // For each pixel (i,j)
  for(unsigned i=0; i<height; ++i)
  {
    if (i % 50 == 0) {
      std::cout << "Rendering row " << i << "/" << height << std::endl;
    }

    for(unsigned j=0; j<width; ++j)
    {
      // Accumulate color from multiple samples
      Eigen::Vector3d rgb(0,0,0);

      // Multiple samples per pixel for depth of field
      for (int s = 0; s < samples_per_pixel; ++s) {
        Eigen::Vector3d sample_color(0,0,0);

        // Random jitter for antialiasing and lens sampling
        double u = uniform(rng);
        double v = uniform(rng);

        Ray ray;
        if (camera.aperture > 0.0) {
          // Use depth of field
          viewing_ray_dof(camera, i, j, width, height, u, v, ray);
        } else {
          // Standard pinhole camera
          viewing_ray(camera, i, j, width, height, ray);
        }

        // Shoot ray and collect color
        raycolor(ray, 1.0, objects, lights, 0, sample_color);
        rgb += sample_color;
      }

      // Average the samples
      rgb /= double(samples_per_pixel);

      // Apply film photography post-processing effects
      rgb = apply_warm_grading(rgb, 0.3);        // Warm vintage look
      rgb = apply_vignetting(rgb, i, j, width, height, 0.6);  // Stronger lens vignetting
      rgb = apply_film_grain(rgb, i, j, 0.025);   // More visible film grain

      // Write double precision color into image
      auto clamp = [](double s){ return std::max(std::min(s,1.0),0.0);};
      rgb_image[0+3*(j+width*i)] = 255.0*clamp(rgb(0));
      rgb_image[1+3*(j+width*i)] = 255.0*clamp(rgb(1));
      rgb_image[2+3*(j+width*i)] = 255.0*clamp(rgb(2));
    }
  }

  std::cout << "Writing output..." << std::endl;
  write_ppm("piece.ppm",rgb_image,width,height,3);
  write_png("piece.png",rgb_image,width,height);
  std::cout << "Done! Output written to piece.ppm and piece.png" << std::endl;

  // Auto-open the image on Windows
  #ifdef _WIN32
  std::cout << "Opening image..." << std::endl;
  system("start piece.png");
  #endif
}
