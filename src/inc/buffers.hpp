#pragma once

#include "inc/glsl.hpp"

/* clang-format off */
#define tex_voxel                       shadowcolor0
#define tex_voxel_depth                 shadowtex0
#define tex_atlas_diffuse               depthtex1
#define tex_atlas_specular              shadowtex1
#define tex_atlas_normal                depthtex2

#define tex_gbuffer_depth               depthtex0
#define tex_gbuffer                     colortex0
#define tex_worldpos                    colortex1
#define tex_gbuffer_history             colortex2
#define tex_worldpos_history            colortex3
#define tex_motion_vector               colortex4
#define tex_moments_history             colortex5
#define tex_diffuse_direct              colortex6
#define tex_diffuse_indirect            colortex7
#define tex_diffuse_direct_history      colortex8
#define tex_diffuse_indirect_history    colortex9
/* clang-format on */

const bool colortex0Clear   = true;
const bool colortex1Clear   = true;
const bool colortex2Clear   = false;
const bool colortex3Clear   = false;
const bool colortex4Clear   = true;
const bool colortex5Clear   = false;
const bool colortex6Clear   = true;
const bool colortex7Clear   = true;
const bool colortex8Clear   = false;
const bool colortex9Clear   = false;
const bool shadowcolorClear = true;

const vec4 shadowcolorClearColor = vec4(0, 0, 0, 1);

#ifndef RGBA32F
#define RGBA32F 0
#endif
#ifndef RGBA32U
#define RGBA32U 0
#endif

const int shadowcolor0Format = RGBA32F;
const int colortex0Format    = RGBA32F;
const int colortex1Format    = RGBA32F;
const int colortex2Format    = RGBA32F;
const int colortex3Format    = RGBA32F;
const int colortex4Format    = RGBA32F;
const int colortex5Format    = RGBA32F;
const int colortex6Format    = RGBA32F;
const int colortex7Format    = RGBA32F;
const int colortex8Format    = RGBA32F;
const int colortex9Format    = RGBA32F;
