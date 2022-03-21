#pragma once
#include "inc/glsl.hpp"

attribute vec4 at_tangent;
attribute vec3 mc_Entity;
attribute vec2 mc_midTexCoord;

#include "inc/constants.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

flat out uint blockID;      // 方块种类
out vec2      texCoord;     // 纹理坐标
out vec3      color;        // 顶点色
out vec3      normal;       // 面法线(世界坐标)
out vec3      tangent;      // 面切线(世界坐标)
out vec3      worldPos;     // 世界坐标
out vec3      motionVector; // 运动向量

void main() {
    blockID     = getBlockID(mc_Entity);
    texCoord    = gl_MultiTexCoord0.st;
    color       = gl_Color.rgb;
    normal      = normalize(mat3(gbufferModelViewInverse) * gl_NormalMatrix * gl_Normal);
    tangent     = at_tangent.xyz;
    worldPos    = (gbufferModelViewInverse * gl_ModelViewMatrix * gl_Vertex).xyz + cameraPosition.xyz;
    gl_Position = gbufferProjection * gl_ModelViewMatrix * gl_Vertex;
}