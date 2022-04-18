#pragma once

#include "inc/glsl.hpp"

/* clang-format off */
#define tex_shadowcolor                 shadowcolor0
#define tex_voxel                       shadowcolor0
#define tex_shadowdepth                shadowtex0
#define tex_voxel_depth                 shadowtex0
#define tex_atlas_diffuse               depthtex1
#define tex_atlas_specular              shadowtex1
#define tex_atlas_normal                depthtex2

#define tex_gbuffer_depth               depthtex0
#define tex_gbuffer                     colortex0
#define tex_localpos                    colortex1
#define tex_gbuffer_history             colortex2
#define tex_localpos_history            colortex3
#define tex_motion_vector               colortex4
#define tex_moments_history             colortex5 // HRR
#define tex_diffuse_direct              colortex6 // HRR
#define tex_diffuse_direct_history      colortex7 // HRR
/* clang-format on */

const bool colortex0Clear    = true;
const bool colortex1Clear    = true;
const bool colortex2Clear    = false;
const bool colortex3Clear    = false;
const bool colortex4Clear    = true;
const bool colortex5Clear    = false;
const bool colortex6Clear    = true;
const bool colortex7Clear    = false;
const bool shadowcolor0Clear = true;
const bool shadowcolor1Clear = true;

const vec4 shadowcolor0ClearColor = vec4(0, 0, 0, 0);
const vec4 shadowcolor1ClearColor = vec4(0, 0, 0, 0);

#ifndef RGBA32F
#define RGBA32F 0
#endif
#ifndef RGBA32U
#define RGBA32U 0
#endif
#ifndef RGBA16F
#define RGBA16F 0
#endif

const int shadowcolor0Format = RGBA32F;
const int shadowcolor1Format = RGBA32F;
const int colortex0Format    = RGBA32F;
const int colortex1Format    = RGBA16F;
const int colortex2Format    = RGBA32F;
const int colortex3Format    = RGBA16F;
const int colortex4Format    = RGBA16F;
const int colortex5Format    = RGBA16F;
const int colortex6Format    = RGBA16F;
const int colortex7Format    = RGBA16F;
const int colortex13Format   = RGBA32F;
const int colortex14Format   = RGBA32F;
const int colortex15Format   = RGBA32F;

const bool shadowtex0Mipmap         = true;
const bool shadowHardwareFiltering0 = true;
const bool shadowtex0Nearest        = false;