#pragma once
#include "inc/glsl.hpp"

in vec4 at_tangent;
in vec3 mc_Entity;
in vec2 mc_midTexCoord;

#ifdef GBUFFERS_ENTITIES
in vec3 at_velocity;
#endif

#include "inc/constants.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

flat out uint blockID;  // 方块种类
out vec2      texCoord; // 纹理坐标
out vec3      color;    // 顶点色
out vec3      normal;   // 面法线(世界坐标)
out vec3      tangent;  // 面切线(世界坐标)
out vec3      localPos; // 世界坐标减摄像机位置
out vec3      velocity; // 运动向量

void main() {
    blockID  = getBlockID(mc_Entity);
    texCoord = gl_MultiTexCoord0.st;
    color    = gl_Color.rgb;
    normal   = normalize(mat3(gbufferModelViewInverse) * gl_NormalMatrix * gl_Normal);
    tangent  = at_tangent.xyz;
    localPos = (gbufferModelViewInverse * gl_ModelViewMatrix * gl_Vertex).xyz;
#ifdef GBUFFERS_ENTITIES
    velocity = at_velocity;
#else
    velocity = vec3(0);
#endif
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}