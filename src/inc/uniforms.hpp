#pragma once
#include "inc/glsl.hpp"

uniform float near;
uniform float far;
uniform vec2  viewSize;
uniform mat4  gbufferProjection;
uniform mat4  gbufferProjectionInverse;
uniform mat4  gbufferModelView;
uniform mat4  gbufferModelViewInverse;
uniform mat4  shadowProjection;
uniform mat4  shadowProjectionInverse;
uniform mat4  shadowModelView;
uniform mat4  shadowModelViewInverse;
uniform mat4  gbufferPreviousModelView;
uniform mat4  gbufferPreviousProjection;
uniform vec3  cameraPosition;
uniform vec3  previousCameraPosition;

uniform vec3 sunPosition;
uniform vec3 upPosition;

uniform float sunTemperature;
uniform float sunIntensity;
uniform float sunAngle;
uniform vec3  skyColor;

uniform int worldTime;
uniform int frameCounter;

uniform int   isEyeInWater;
uniform ivec2 eyeBrightness;
uniform ivec2 eyeBrightnessSmooth;
uniform int   fogMode;

uniform int heldBlockLightValue;
uniform int heldBlockLightValue2;
uniform int heldItemId;
uniform int heldItemId2;

uniform vec4 entityColor;
