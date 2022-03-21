#pragma once
#include "inc/glsl.hpp"

const bool  shadowcolorClear      = true;
const vec4  shadowcolorClearColor = vec4(0, 0, 0, 1);
const int   shadowMapResolution   = 8192;
const float shadowDistance        = 128;

uniform mat4 gbufferProjection;
uniform mat4 gbufferProjectionInverse;
uniform mat4 gbufferModelView;
uniform mat4 gbufferModelViewInverse;
uniform mat4 shadowProjection;
uniform mat4 shadowProjectionInverse;
uniform mat4 shadowModelView;
uniform mat4 shadowModelViewInverse;
uniform mat4 gbufferPreviousModelView;
uniform mat4 gbufferPreviousProjection;

uniform vec3 sunPosition;
uniform vec3 upPosition;

uniform vec3 cameraPosition;
uniform vec3 previousCameraPosition;

uniform float near;
uniform float far;
uniform float viewWidth;
uniform float viewHeight;
uniform float rainStrength;
uniform float wetness;
uniform float aspectRatio;
uniform int   worldTime;
uniform float frameTimeCounter;
uniform float frameTime;
uniform int   frameCounter;
uniform float sunAngle;
uniform vec3  skyColor;

uniform int   isEyeInWater;
uniform ivec2 eyeBrightness;
uniform ivec2 eyeBrightnessSmooth;
uniform int   fogMode;

uniform int heldBlockLightValue;
uniform int heldBlockLightValue2;
uniform int heldItemId;
uniform int heldItemId2;

uniform vec4 entityColor;
uniform vec2 viewSize;
