#pragma message("Compiling precompiled headers.\n")
// Constants
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1080
#define SCREEN_POSX 300
#define SCREEN_POSY 100
#define RAYSCREEN_WIDTH 770
#define RAYSCREEN_HEIGHT 1080
#define PI 3.1415926
#define E 2.7182818

#define MALLOC64( x ) _aligned_malloc( x, 64 )

#define ALPHA_MASK  0xff000000
#define RED_MASK  0x00ff0000
#define GREEN_MASK  0x0000ff00
#define BLUE_MASK  0x000000ff

#define RAYTRACER_BLOCK_SIZE 100
#define RAYTRACER_RECURSION_DEPTH 3
#define RAYTRACER_MAX_RENDERDISTANCE 100
#define RAY_MIGRAINE 0.0001F
#define BLINN_PHONG_POWER 130

#define SIMD 1

#ifdef _WIN32
#include <Windows.h>
#endif

// Flags
#define CHECK_PERFORMANCE

// default headers
#include<stdio.h>
#include<vector>
typedef unsigned char Pixel;
#include <math.h>  
#include <assert.h>
#include <string>
#include <memory>
#include <algorithm>

// dependency headers
#define SDL_MAIN_HANDLED
#include<SDL.h>
#include<vectorclass.h>
#include<vector3d.h>
// local headers
#include "Utils.h"
// Drawing
#include "Window.h"
#include "Screen.h"
#include "Renderer.h"
#include "RaytracerRenderer.h"
#include "Camera.h"
// Objects
#include "Light.h"
#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"
// Scene logic
#include "Scene.h"
// SIMD
#include "RaySystem.h"