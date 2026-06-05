# SnowEngine

A custom game engine written in C++ and OpenGL, developed to explore modern rendering techniques, engine architecture, and low-level graphics programming.

## Features

### Rendering

* Deferred Rendering
* Physically Based Rendering (PBR)
* HDR Rendering & Tone Mapping
* Image-Based Lighting (IBL)
* SSAO (Screen Space Ambient Occlusion)
* Shadow Mapping

  * Directional Shadows
  * Omnidirectional Point Shadows
* Cubemaps & Skyboxes
* Parallax Mapping
* Normal Mapping

### Assets

* Model Loading (Assimp)
* Texture Loading (stb_image)
* Text Rendering (FreeType)

### Engine Systems

* Scene Management
<!-- * Material System -->
<!-- * Asset Management -->
* Entity Component System (In Progress)

<!-- ## Screenshots -->

<!-- (Add screenshots here) -->

## Technologies

* C++
* OpenGL 4.5
* GLFW
* GLAD
* GLM
* Assimp
* stb_image
* FreeType

## Building

### Linux

```bash
git clone <repo-url>
cd SnowEngine

mkdir build
cd build

cmake ..
make
./SnowEngine
```

## Learning Goals

This project is being developed from scratch as a learning-focused engine to explore:

* Graphics Programming
* Rendering Pipelines
* Engine Architecture
* Data-Oriented Design
* ECS
* Multithreading
* Systems Programming

## Future Work

* ECS Architecture
* Asset Pipeline
* Multithreading
<!-- * Animation System -->
* Physics Integration
* Vulkan Backend Exploration

<!-- ## License -->

<!-- MIT License -->
