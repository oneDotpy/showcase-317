#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <Eigen/Core>

#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "read_json.h"
#include "raycolor.h"
#include "viewing_ray_dof.h"
#include "viewing_ray.h"
#include "post_process.h"

// Render state
struct RenderState {
  double aperture = 0.12;
  double focal_distance = 3.3;
  bool enable_grain = true;
  bool enable_vignette = true;
  bool enable_grading = true;
  int samples_per_pixel = 8;  // Lower for interactive speed
  bool needs_render = true;
};

RenderState g_state;


// Framebuffer size callback
void framebuffer_size_callback(GLFWwindow* window, int fb_width, int fb_height) {
  (void)window;
  glViewport(0, 0, fb_width, fb_height);
}

// Keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode; (void)mods;

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_UP:
        g_state.aperture += 0.02;
        g_state.needs_render = true;
        std::cout << "Aperture: " << g_state.aperture << std::endl;
        break;
      case GLFW_KEY_DOWN:
        g_state.aperture = std::max(0.0, g_state.aperture - 0.02);
        g_state.needs_render = true;
        std::cout << "Aperture: " << g_state.aperture << std::endl;
        break;
      case GLFW_KEY_RIGHT:
        g_state.focal_distance += 0.2;
        g_state.needs_render = true;
        std::cout << "Focal distance: " << g_state.focal_distance << std::endl;
        break;
      case GLFW_KEY_LEFT:
        g_state.focal_distance = std::max(0.1, g_state.focal_distance - 0.2);
        g_state.needs_render = true;
        std::cout << "Focal distance: " << g_state.focal_distance << std::endl;
        break;
      case GLFW_KEY_G:
        g_state.enable_grain = !g_state.enable_grain;
        g_state.needs_render = true;
        std::cout << "Film grain: " << (g_state.enable_grain ? "ON" : "OFF") << std::endl;
        break;
      case GLFW_KEY_V:
        g_state.enable_vignette = !g_state.enable_vignette;
        g_state.needs_render = true;
        std::cout << "Vignette: " << (g_state.enable_vignette ? "ON" : "OFF") << std::endl;
        break;
      case GLFW_KEY_C:
        g_state.enable_grading = !g_state.enable_grading;
        g_state.needs_render = true;
        std::cout << "Color grading: " << (g_state.enable_grading ? "ON" : "OFF") << std::endl;
        break;
      case GLFW_KEY_Q:
        g_state.samples_per_pixel = std::max(1, g_state.samples_per_pixel / 2);
        g_state.needs_render = true;
        std::cout << "Samples: " << g_state.samples_per_pixel << std::endl;
        break;
      case GLFW_KEY_W:
        g_state.samples_per_pixel = std::min(128, g_state.samples_per_pixel * 2);
        g_state.needs_render = true;
        std::cout << "Samples: " << g_state.samples_per_pixel << std::endl;
        break;
      case GLFW_KEY_R:
        g_state.needs_render = true;
        std::cout << "Re-rendering..." << std::endl;
        break;
    }
  }
}

// Load shader from file
std::string load_shader_source(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open shader: " << path << std::endl;
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// Compile shader
GLuint compile_shader(GLenum type, const std::string& source) {
  GLuint shader = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info[512];
    glGetShaderInfoLog(shader, 512, nullptr, info);
    std::cerr << "Shader compilation failed:\n" << info << std::endl;
    return 0;
  }
  return shader;
}

// Create shader program
GLuint create_program(const std::string& vert_path, const std::string& frag_path) {
  std::string vert_src = load_shader_source(vert_path);
  std::string frag_src = load_shader_source(frag_path);

  if (vert_src.empty() || frag_src.empty()) {
    return 0;
  }

  GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_src);
  GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_src);

  if (vert == 0 || frag == 0) {
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char info[512];
    glGetProgramInfoLog(program, 512, nullptr, info);
    std::cerr << "Shader linking failed:\n" << info << std::endl;
    return 0;
  }

  glDeleteShader(vert);
  glDeleteShader(frag);

  return program;
}

// Render the scene
void render_scene(
  Camera& camera,
  const std::vector<std::shared_ptr<Object>>& objects,
  const std::vector<std::shared_ptr<Light>>& lights,
  int width, int height,
  std::vector<uint8_t>& rgb_image)
{
  camera.aperture = g_state.aperture;
  camera.focal_distance = g_state.focal_distance;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      Eigen::Vector3d color(0, 0, 0);

      for (int s = 0; s < g_state.samples_per_pixel; ++s) {
        double u = (i + ((double)rand() / RAND_MAX)) / width;
        double v = (j + ((double)rand() / RAND_MAX)) / height;

        Ray ray;
        if (camera.aperture > 0.0) {
          viewing_ray_dof(camera, i, j, width, height, u, v, ray);
        } else {
          viewing_ray(camera, i, j, width, height, ray);
        }

        Eigen::Vector3d ray_color;
        raycolor(ray, 1.0, objects, lights, 0, ray_color);
        color += ray_color;
      }

      color /= g_state.samples_per_pixel;

      // Post-processing
      if (g_state.enable_grading) {
        color = apply_warm_grading(color, 0.25);
      }
      if (g_state.enable_vignette) {
        color = apply_vignetting(color, i, j, width, height, 0.8);
      }
      if (g_state.enable_grain) {
        color = apply_film_grain(color, i, j, 0.05);
      }

      // Flip Y for OpenGL texture
      int idx = (i * width + j) * 3;
      rgb_image[idx + 0] = (uint8_t)std::min(255.0, std::max(0.0, color(0) * 255.0));
      rgb_image[idx + 1] = (uint8_t)std::min(255.0, std::max(0.0, color(1) * 255.0));
      rgb_image[idx + 2] = (uint8_t)std::min(255.0, std::max(0.0, color(2) * 255.0));
    }
  }
}

int main(int argc, char* argv[]) {
  std::string scene_file = argc > 1 ? argv[1] : "data/showcase.json";

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  // OpenGL 3.3 Core
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int width = 640;
  int height = 360;

  GLFWwindow* window = glfwCreateWindow(width, height, "Film Camera Dreams - Interactive", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Load scene
  Camera camera;
  std::vector<std::shared_ptr<Object>> objects;
  std::vector<std::shared_ptr<Light>> lights;
  read_json(scene_file, camera, objects, lights);

  g_state.aperture = camera.aperture;
  g_state.focal_distance = camera.focal_distance;

  std::cout << "\n========================================" << std::endl;
  std::cout << "  Film Camera Dreams - Interactive Mode" << std::endl;
  std::cout << "========================================\n" << std::endl;
  std::cout << "CONTROLS:" << std::endl;
  std::cout << "  UP/DOWN    - Adjust aperture (bokeh strength)" << std::endl;
  std::cout << "  LEFT/RIGHT - Adjust focal distance" << std::endl;
  std::cout << "  G          - Toggle film grain" << std::endl;
  std::cout << "  V          - Toggle vignetting" << std::endl;
  std::cout << "  C          - Toggle color grading" << std::endl;
  std::cout << "  Q/W        - Decrease/Increase samples (quality)" << std::endl;
  std::cout << "  R          - Force re-render" << std::endl;
  std::cout << "  ESC        - Quit\n" << std::endl;

  // Create shader program
  GLuint program = create_program("shaders/display.vert", "shaders/display.frag");
  if (program == 0) {
    std::cerr << "Failed to create shader program" << std::endl;
    return -1;
  }

  // Fullscreen quad vertices
  float quad_vertices[] = {
    // pos        // tex
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
  };

  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Create texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  std::vector<uint8_t> rgb_image(width * height * 3, 0);

  // Set viewport to match actual framebuffer size
  int fb_width, fb_height;
  glfwGetFramebufferSize(window, &fb_width, &fb_height);
  std::cout << "Render size: " << width << "x" << height << std::endl;
  std::cout << "Framebuffer size: " << fb_width << "x" << fb_height << std::endl;
  std::cout << "Camera image plane: " << camera.width << "x" << camera.height << std::endl;
  glViewport(0, 0, fb_width, fb_height);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    if (g_state.needs_render) {
      std::cout << "Rendering..." << std::flush;
      render_scene(camera, objects, lights, width, height, rgb_image);

      // Upload to texture
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_image.data());

      g_state.needs_render = false;
      std::cout << " Done!" << std::endl;
    }

    // Render quad with texture (with letterboxing to preserve aspect ratio)
    int current_fb_width, current_fb_height;
    glfwGetFramebufferSize(window, &current_fb_width, &current_fb_height);
    
    // Calculate aspect ratios
    float render_aspect = (float)width / (float)height;
    float window_aspect = (float)current_fb_width / (float)current_fb_height;
    
    int viewport_x = 0, viewport_y = 0;
    int viewport_width = current_fb_width;
    int viewport_height = current_fb_height;
    
    // Letterbox to maintain aspect ratio
    if (window_aspect > render_aspect) {
      // Window is wider - add black bars on sides
      viewport_width = (int)(current_fb_height * render_aspect);
      viewport_x = (current_fb_width - viewport_width) / 2;
    } else {
      // Window is taller - add black bars top/bottom
      viewport_height = (int)(current_fb_width / render_aspect);
      viewport_y = (current_fb_height - viewport_height) / 2;
    }
    
    glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteTextures(1, &texture);
  glDeleteProgram(program);

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
