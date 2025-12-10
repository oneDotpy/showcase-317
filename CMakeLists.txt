cmake_minimum_required(VERSION 3.20)
project(raytracing)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PIC
# This sets -fPIC (or equivalent) where needed in a target-scoped way.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# OpenGL
if(NOT CMAKE_VERSION VERSION_LESS "3.11")
  cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)

# Include GLFW and GLAD cmake projects
if(NOT TARGET glfw)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")
endif()
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glad")

# Paths
set(ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Use solution or src
option(USE_SOLUTION "Build the reference solution" OFF)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
if (USE_SOLUTION)
  set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/solution")
endif()

# Sources
# Collect all .cpp in src/ or solution/
file(GLOB SRCFILES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")

# Homework library sources that should *not* be part of the main executable
# if linking with a prebuild library.
set(HW2FILES
  "${SRC_DIR}/Plane.cpp"
  "${SRC_DIR}/Sphere.cpp"
  "${SRC_DIR}/Triangle.cpp"
  "${SRC_DIR}/TriangleSoup.cpp"
  "${SRC_DIR}/first_hit.cpp"
  "${SRC_DIR}/viewing_ray.cpp"
  "${SRC_DIR}/write_ppm.cpp"
)
list(REMOVE_ITEM SRCFILES ${HW2FILES})

# Batch renderer executable (original main.cpp)
set(BATCH_SRCFILES ${SRCFILES})
list(APPEND BATCH_SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Interactive viewer executable (main_interactive.cpp)
set(INTERACTIVE_SRCFILES ${SRCFILES})
list(APPEND INTERACTIVE_SRCFILES "${CMAKE_CURRENT_SOURCE_DIR}/main_interactive.cpp")

# Allow caller to provide extra sources (e.g., libigl helpers)
set(EXTRA_SOURCES "")
if (DEFINED LIBIGL_EXTRA_SOURCES)
  list(APPEND EXTRA_SOURCES ${LIBIGL_EXTRA_SOURCES})
endif()

# Batch renderer executable target
add_executable(${PROJECT_NAME} ${BATCH_SRCFILES} ${EXTRA_SOURCES})

# Interactive viewer executable target
add_executable(${PROJECT_NAME}_interactive ${INTERACTIVE_SRCFILES} ${EXTRA_SOURCES})

# Include paths (target-scoped). Mark third-party as SYSTEM to reduce warnings.
target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${ROOT}/include"
)
target_include_directories(${PROJECT_NAME}_interactive
  PRIVATE
    "${ROOT}/include"
)
if (EXISTS "${ROOT}/eigen")
  target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "${ROOT}/eigen")
  target_include_directories(${PROJECT_NAME}_interactive SYSTEM PRIVATE "${ROOT}/eigen")
endif()
if (EXISTS "${ROOT}/json")
  target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "${ROOT}/json")
  target_include_directories(${PROJECT_NAME}_interactive SYSTEM PRIVATE "${ROOT}/json")
endif()

# ---- Optional external hw2 library support ----
# If you want to link against a prebuilt hw2 in a directory:
#   cmake -DCMAKE_BUILD_TYPE=Debug -DHW2LIB_DIR=../lib/debug/linux/ ..
set(HW2LIB_DIR  "" CACHE PATH   "Directory containing prebuilt hw2 library (optional)")
set(HW2LIB_NAME "hw2" CACHE STRING "Name of the prebuilt hw2 library (optional)")

if (HW2LIB_DIR)
  message(STATUS "Using HW2LIB_DIR: ${HW2LIB_DIR}, HW2LIB_NAME: ${HW2LIB_NAME}")
  # Link an external lib named HW2LIB_NAME from HW2LIB_DIR
  target_link_directories(${PROJECT_NAME} PRIVATE "${HW2LIB_DIR}")
  target_link_directories(${PROJECT_NAME}_interactive PRIVATE "${HW2LIB_DIR}")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${HW2LIB_NAME})
  target_link_libraries(${PROJECT_NAME}_interactive PRIVATE ${HW2LIB_NAME})
else()
  message(STATUS "No HW2LIB_DIR provided, building hw2 from source.")
  # Build our own hw2 library from the HW2FILES and link it
  # We're readding the files we removed from above.
  add_library(hw2 STATIC ${HW2FILES})

  # Inherit include paths for third-party headers
  target_include_directories(hw2
    PRIVATE
      "${ROOT}/include"
  )
  if (EXISTS "${ROOT}/eigen")
    target_include_directories(hw2 SYSTEM PRIVATE "${ROOT}/eigen")
  endif()
  if (EXISTS "${ROOT}/json")
    target_include_directories(hw2 SYSTEM PRIVATE "${ROOT}/json")
  endif()

  # Link the hw2 library to the main executable.
  target_link_libraries(${PROJECT_NAME} PRIVATE hw2)
  target_link_libraries(${PROJECT_NAME}_interactive PRIVATE hw2)
endif()

# Link OpenGL libraries for interactive viewer
target_link_libraries(${PROJECT_NAME}_interactive
  PRIVATE
    glfw
    glad
    OpenGL::GL
)

# Add STB for image display (header-only)
option(ENABLE_VIEWER "Enable live preview window" ON)
if(ENABLE_VIEWER)
  add_definitions(-DENABLE_VIEWER)
endif()

# Warnings
if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
  target_compile_options(${PROJECT_NAME}_interactive PRIVATE /W4 /permissive-)
  if (TARGET hw2)
    target_compile_options(hw2 PRIVATE /W4 /permissive-)
  endif()
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(${PROJECT_NAME}_interactive PRIVATE -Wall -Wextra -Wpedantic)
  if (TARGET hw2)
    target_compile_options(hw2 PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

