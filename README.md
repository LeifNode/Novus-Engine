# Novus Engine

## Introduction

**Novus Engine** is a bare-bones 3D game engine built using Direct3D 11. This is made with the intent of helping me experiment with complex shaders and effects.

Currently I am porting code over from my old project into this new engine and refactoring it as I go in order to be more consistent with the coding style as a whole.

This project is built in Visual Studio 2013 and is currently only targeted at x64 hardware.

## Features (Planned)

#### Graphics
* Cascaded Shadow Mapping
* High Dynamic Range Rendering
* Tiled Deferred Shading Pipeline
* Physically Based Shading BRDF (based on Unreal Engine 4's)
* Voxel Cone Traced Global Illumination

#### GUI
* A GUI system that can interchangeably be attached to the camera and be interracted with in screen space or positioned in the world an interacted with in 3D. 

#### Physics
* Simple Rigid Body Physics System
