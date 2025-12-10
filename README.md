# Film Camera Dreams: A Ray-Traced Cherry Blossom Scene
### CSC317 Fall 2025 Showcase Submission

## Personal Information
- **Name:** Ahnaf Keenan Ardhito
- **UtorID:** ardhitoa
- **Student Number:** 1010236219
- **Base Assignment:** A3 (Ray Tracing)

---

## Description

This piece extends the basic ray tracing assignment (A3) with photography-inspired camera effects combined with a serene sakura garden scene. The goal was to create a render that captures the delicate beauty of cherry blossoms in full bloom with the aesthetic character of classic film photography through physically-based depth of field, film grain, warm color grading, and vignetting.

The scene features:
- **Cherry Blossom Trees:** 13 custom-modeled sakura trees with natural branching structures
  - 4 detailed foreground trees with organic branch splits and tapering trunks
  - 9 background trees at varying depths for atmospheric perspective
- **Natural Branching:** Trees built from spheres positioned at varying offsets to create realistic branch splits (not simple stacked circles)
- **Vibrant Pink Blossoms:** Cherry pink and soft pink materials with carefully tuned color values for authentic sakura appearance
- **Fallen Petals:** 10 scattered petal spheres on the grass for added detail
- **Lush Grass:** Ground plane with natural green grass material
- **Atmospheric Sky:** Soft blue sky backdrop
- **Depth Composition:** Strategic use of depth of field with foreground trees in gentle focus and background trees creating depth

### Features Added

#### 1. **Depth of Field (DOF) with Bokeh** ⭐ Primary Feature
- **Implementation:** [src/viewing_ray_dof.cpp](src/viewing_ray_dof.cpp), [include/viewing_ray_dof.h](include/viewing_ray_dof.h)
- **Description:** Implements the thin lens camera model to simulate realistic depth of field effects
  - Aperture control (simulating f-stop on real cameras)
  - Focal distance control (focus plane)
  - Concentric disk sampling for uniform lens sampling ([src/random_disk_sample.cpp](src/random_disk_sample.cpp))
  - Multiple rays per pixel (32 samples/pixel for quality)
  - Natural bokeh (out-of-focus blur) that varies with distance from focal plane

**Technical Details:**
- Instead of a single ray through the pixel center (pinhole camera), each pixel shoots multiple rays from random points on a circular lens aperture
- All rays converge at the focal plane (objects at focal distance are sharp)
- Objects closer or farther than the focal distance naturally blur based on the circle of confusion
- Uses the Shirley-Chiu concentric mapping algorithm for better lens sample distribution

**Code Location:**
- Camera parameters extended in [include/Camera.h](include/Camera.h) with `aperture` (0.11) and `focal_distance` (10.0)
- Main rendering loop in [main.cpp](main.cpp) lines 58-77 handles multi-sampling
- DOF ray generation in [src/viewing_ray_dof.cpp](src/viewing_ray_dof.cpp)

#### 2. **Film Grain**
- **Implementation:** [src/post_process.cpp](src/post_process.cpp) - `apply_film_grain()`
- **Description:** Adds subtle randomized noise to each pixel to simulate the organic grain structure of analog film
  - Per-channel noise for realistic color grain
  - Deterministic pseudo-random function based on pixel coordinates (same seed = same grain pattern)
  - Intensity: 0.025 (subtle but noticeable)

**Code Location:** Lines 19-36 in [post_process.cpp](src/post_process.cpp), applied in [main.cpp](main.cpp) line 86

#### 3. **Warm Color Grading**
- **Implementation:** [src/post_process.cpp](src/post_process.cpp) - `apply_warm_grading()`
- **Description:** Applies a vintage warm color tone reminiscent of classic film stocks
  - Boosts red/yellow channels slightly
  - Reduces blue channel for warmth
  - Adds subtle contrast enhancement for that "film look"
  - Intensity: 0.3 for gentle warmth
  - Inspired by Kodak Portra and Fujifilm color profiles

**Code Location:** Lines 62-86 in [post_process.cpp](src/post_process.cpp), applied in [main.cpp](main.cpp) line 84

#### 4. **Vignetting**
- **Implementation:** [src/post_process.cpp](src/post_process.cpp) - `apply_vignetting()`
- **Description:** Simulates natural lens vignetting where image corners darken
  - Distance-based radial falloff from center
  - Smooth gradient using smoothstep function
  - Aspect-ratio aware (circular vignetting in rectangular frame)
  - Strength: 0.6 for noticeable but natural effect
  - Mimics optical vignetting in real camera lenses

**Code Location:** Lines 38-60 in [post_process.cpp](src/post_process.cpp), applied in [main.cpp](main.cpp) line 85

#### 5. **Custom Procedural Tree Modeling**
- **Implementation:** Scene definition in [data/showcase.json](data/showcase.json)
- **Description:** Hand-crafted sakura trees built entirely from sphere primitives with natural branching
  - Each foreground tree has 4-7 trunk spheres tapering upward
  - Branches split at varying angles and offsets for organic appearance
  - 8-12 blossom clusters per tree positioned at branch ends
  - Background trees scaled smaller for atmospheric perspective
  - Mix of "cherry pink" and "soft pink" materials for color variation

**Technical Details:**
- Foreground trees positioned at z = -3 to -7
- Background trees positioned at z = -5 to -14 with emphasis on far right
- Trunk spheres: 0.2-0.3 radius, brown bark material
- Blossom spheres: 0.4-0.6 radius, vibrant pink materials
- Branch offsets: ±0.3 to ±1.0 units in x/z for natural spread

#### 6. **Carefully Tuned Materials**
- **Cherry Pink Material:** Vibrant pink with high reflectance
  - ka: [1.0, 0.7, 0.8], kd: [1.0, 0.75, 0.85]
  - ks: [0.7, 0.6, 0.6], phong_exponent: 130
  - Creates soft, delicate petal appearance with subtle highlights

- **Lighting Balance:** Directional light reduced to [0.7, 0.68, 0.65] to prevent overexposure while maintaining natural outdoor lighting

#### 7. **Enhanced Rendering Settings**
- **Implementation:** [main.cpp](main.cpp)
- **Description:** High-quality rendering with optimal parameters
  - Resolution: 1280x720 (HD)
  - Samples per pixel: 32 (smooth depth of field)
  - Progress reporting during render
  - Monte Carlo sampling with fixed seed for reproducible results

---

## Instructions

### Prerequisites
- CMake 3.20 or higher
- C++17 compatible compiler (MSVC, GCC, or Clang)
- Eigen library (included)
- nlohmann/json library (included)
- Python 3 with PIL/Pillow (for optional text overlay)

### Building and Running

**IMPORTANT:** All commands should be run from the main `showcase/` directory unless otherwise specified.

#### 1. Build the Project

**Windows PowerShell:**
```powershell
# Replace XX with your Visual Studio version (e.g., 18, 22)
& "C:\Program Files\Microsoft Visual Studio\XX\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release
```

**Linux/Mac:**
```bash
cmake --build build --config Release
```

This builds both the batch renderer (`raytracing.exe`) and interactive viewer (`raytracing_interactive.exe`).

To build only specific targets:
```powershell
# Windows - Build only batch renderer
& "C:\Program Files\Microsoft Visual Studio\XX\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release --target raytracing

# Windows - Build only interactive viewer
& "C:\Program Files\Microsoft Visual Studio\XX\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release --target raytracing_interactive
```

```bash
# Linux/Mac - Build only batch renderer
cmake --build build --config Release --target raytracing

# Linux/Mac - Build only interactive viewer
cmake --build build --config Release --target raytracing_interactive
```

#### 2. Run the Batch Renderer

**From main directory:**

**Windows PowerShell:**
```powershell
.\build\Release\raytracing.exe data\showcase.json
```

**Linux/Mac:**
```bash
./build/Release/raytracing data/showcase.json
```

This will generate `piece.ppm` in the `build/Release/` directory.

#### 3. Run the Interactive Viewer (Real-time Preview)

**From main directory:**

**Windows PowerShell:**
```powershell
.\build\Release\raytracing_interactive.exe data\showcase.json
```

**Linux/Mac:**
```bash
./build/Release/raytracing_interactive data/showcase.json
```

**Interactive Controls:**
- **UP/DOWN** - Adjust aperture (bokeh strength)
- **LEFT/RIGHT** - Adjust focal distance (what's in focus)
- **G** - Toggle film grain effect
- **V** - Toggle vignetting effect
- **C** - Toggle warm color grading
- **Q/W** - Decrease/Increase samples per pixel (quality vs speed)
- **R** - Force re-render
- **ESC** - Quit

The viewer starts with low sample count (8 samples) for fast iteration. Press W to increase quality.

### Adjusting Camera Settings

Edit `data/showcase.json` to modify the scene and camera:

```json
{
  "camera": {
    "aperture": 0.11,        // Larger = more blur (try 0.05-0.2)
    "focal_distance": 10.0,  // Distance to focus plane
    "eye": [2, 2.2, 10],     // Camera position
    "look": [0, 0, -1]       // Look direction
  }
}
```

Adjust render quality in `main.cpp`:
```cpp
int width =  1280;  // Resolution width
int height = 720;   // Resolution height
int samples_per_pixel = 32;  // More = better quality, slower
```

### Render Time Estimates
- **640x360, 16 samples:** ~10-30 seconds
- **1280x720, 32 samples:** ~3-7 minutes (current settings)
- **1920x1080, 64 samples:** ~15-25 minutes (maximum quality)

---

## Scene Description

The showcase scene ("Sakura Garden Dreams") recreates a tranquil cherry blossom garden in full spring bloom. The scene is composed entirely from sphere primitives arranged to create natural-looking trees:

**Foreground Sakura Trees (4 trees, depth 3-7 units):**
- Tree 1 (center-left): 6-sphere trunk with 3 branch splits, 11 blossom clusters
- Tree 2 (center): 7-sphere trunk with 4 branch levels, 12 blossom clusters
- Tree 3 (right): 5-sphere trunk with asymmetric branching, 9 blossom clusters
- Tree 4 (far left): 4-sphere trunk with low branches, 8 blossom clusters
- Each tree has unique branching pattern - no two trees identical
- Mix of cherry pink and soft pink materials for natural color variation

**Background Sakura Trees (9 trees, depth 5-14 units):**
- Positioned at varying depths to create atmospheric perspective
- Smaller scale (0.6-0.8x) for distance effect
- Concentrated on far right side for fuller composition
- Simpler branching structure (fewer spheres) for performance

**Fallen Petals (10 spheres):**
- Scattered on grass at ground level (y ≈ 0.05)
- Small radius (0.08-0.12) pink spheres
- Adds detail and reinforces spring bloom theme

**Environmental Elements:**
- Grass plane: Natural green diffuse material with subtle specularity
- Sky plane: Soft blue backdrop material
- Directional lighting: Warm sunlight from upper left ([0.7, 0.68, 0.65] intensity)
- Ambient light: Soft fill light to prevent harsh shadows

**Camera Setup:**
- Eye position: [2, 2.2, 10] - slightly elevated, centered viewpoint
- Focal distance: 10.0 units - focused on mid-ground trees
- Aperture: 0.11 - moderate depth of field for dreamy bokeh
- Look direction: straight ahead toward garden
- Creates natural depth: gentle focus on foreground, sharper mid-ground, atmospheric background

The composition demonstrates natural garden aesthetics while showcasing advanced ray tracing and post-processing techniques. The custom tree modeling creates organic, believable sakura trees without requiring complex mesh assets.

---

## Acknowledgements

### Algorithms & Techniques
- **Concentric Disk Sampling:** Shirley, P., & Chiu, K. (2011). "A Low Distortion Map Between Disk and Square"
  https://psgraphics.blogspot.com/2011/01/improved-code-for-concentric-map.html
  Used for uniform sampling of the camera aperture lens.

- **Thin Lens Camera Model:** Pharr, M., Jakob, W., & Humphreys, G. (2016). "Physically Based Rendering: From Theory to Implementation" (3rd ed.)
  Chapter 6: Camera Models
  Used as reference for depth of field implementation.

- **Path Tracing Fundamentals:** Course materials from CSC317 (Ray Tracing assignment)
  Base ray tracing infrastructure, Blinn-Phong shading, and reflection.

### Libraries
- **Eigen:** C++ template library for linear algebra
  http://eigen.tuxfamily.org/
  Used for vector and matrix operations throughout the ray tracer.

- **nlohmann/json:** JSON for Modern C++
  https://github.com/nlohmann/json
  Used for parsing scene description files.

- **GLFW:** Cross-platform OpenGL library
  https://www.glfw.org/
  Used for the interactive viewer window and input handling.

- **GLAD:** OpenGL loader
  https://glad.dav1d.de/
  Used for loading OpenGL functions in the interactive viewer.

### Inspiration
- Classic film photography aesthetics (Kodak Portra, Fujifilm color science)
- Vintage lens characteristics (vignetting, bokeh shapes, film grain)
- Japanese cherry blossom season (hanami tradition)
- Natural garden composition and organic tree forms

### Assets
- All geometric primitives (spheres, planes) are procedurally generated
- No external textures or 3D models used
- Scene composed entirely through JSON specification with procedural materials and lighting
- Custom tree modeling through strategic sphere placement

---

## Technical Highlights

### What Makes This Special

1. **Physically-Based DOF:** Unlike simple blur filters, this implements the actual physical behavior of a camera lens with aperture and focus distance.

2. **Photography-First Design:** Every effect (grain, vignetting, color grading) is inspired by real film camera characteristics, not arbitrary post-processing.

3. **Custom Procedural Modeling:** Sakura trees are hand-crafted from sphere primitives with natural branching patterns, demonstrating that beautiful organic forms can be created without complex meshes.

4. **Stratified Sampling:** Uses Monte Carlo integration with multiple samples per pixel for smooth, noise-free DOF even with relatively few samples.

5. **Modular Architecture:** Post-processing effects are cleanly separated and can be toggled independently or chained together.

6. **Production-Ready:** Parameters are exposed for artistic control, making it easy to create different looks (shallow DOF portrait style, deep focus landscape style, etc.).

### Performance Optimizations

- Fixed-seed random number generation for reproducibility
- Efficient concentric disk sampling (better distribution than rejection sampling)
- Minimal overhead from post-processing (single-pass per pixel)
- Release build optimizations enabled
- Strategic use of sphere count (detailed foreground, simplified background)
---

## License & Usage

This project is submitted for academic evaluation in CSC317. The base ray tracing code structure is adapted from course materials. All photography-inspired extensions (DOF, film grain, color grading, vignetting) and custom scene modeling are original implementations for this showcase.

---
