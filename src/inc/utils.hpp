#pragma once

#include "inc/constants.hpp"

uint getBlockID(vec4 mc_Entity) {
    if (mc_Entity.x < 0) { return 0; }
    return uint(mc_Entity.x);
}
uint getBlockID(vec3 mc_Entity) {
    if (mc_Entity.x < 0) { return 0; }
    return uint(mc_Entity.x);
}
vec3 sRGB2linear(vec3 c) {
    return pow(c, vec3(2.2));
}
vec4 sRGB2linear(vec4 c) {
    return vec4(pow(c.rgb, vec3(2.2)), c.a);
}
vec3 linear2sRGB(vec3 c) {
    return pow(c, vec3(1.f / GAMMA));
}
vec4 linear2sRGB(vec4 c) {
    return vec4(pow(c.rgb, vec3(1.f / GAMMA)), c.a);
}
float luminance(vec3 c) {
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}
float saturate(float x) { return clamp(x, 0.0, 1.0); }

const float pi = 3.1415926535897932;
// vec3 direction = TBN * uniform2dToHemisphere(some_random_uniform2d)
vec3 uniform2dToHemisphere(vec2 r) {
    float phi  = r.x * pi * 2;
    float cphi = cos(phi);
    float sphi = sin(phi);
    float ctheta = r.y;
    float stheta = sqrt(1 - r.y * r.y);
    return vec3(stheta * cphi, stheta * sphi, ctheta);
}

const int maxTexCoord = 1<<12;
const int texCoordClamp = 1<<24;

// 打包 texel 坐标(ivec2) 到单个 float32 中
// 最大支持 4k 分辨率的贴图
float packTexelPos(ivec2 texelPos) {
    ivec2 iflatten = texelPos * ivec2(1, maxTexCoord);
    float fflatten = iflatten.x + iflatten.y;
    float result = texCoordClamp - fflatten;
    return result / texCoordClamp;
}

// 解包 texel 坐标(ivec2)
// 最大支持 4k 分辨率的贴图
ivec2 unpackTexelPos(float packedTexelPos) {
    float result = packedTexelPos * texCoordClamp;
    float fflatten = texCoordClamp - result;
    return ivec2(mod(fflatten, maxTexCoord), floor(fflatten / maxTexCoord));
}

// texel 坐标转 texCoord
vec2 getTexCoordFromTexelPos(ivec2 targetResolution, ivec2 texelPos) {
    return (vec2(texelPos) + .5f) / vec2(targetResolution);
}
// texCoord 转 texel 坐标 
ivec2 getTexelPosFromTexCoord(ivec2 targetResolution, vec2 texCoord) {
    return ivec2(texCoord * vec2(targetResolution));
}

// texel 坐标转 gl_Position
vec4 getGLPositionFromTexelPos(ivec2 targetResolution, ivec2 texelPos, float depth) {
    return vec4(getTexCoordFromTexelPos(targetResolution, texelPos) * 2.f - 1.f, depth, 1.f);
}

// texel 坐标转 texCoord. 适用于 shadow pass
vec2 shadow_getTexCoordFromTexelPos(ivec2 texelPos) {
    return (vec2(texelPos) + 0.5f) / vec2(shadowMapResolution);
}

// texel 坐标转 gl_Position. 适用于 shadow pass
vec4 shadow_getGLPositionFromTexelPos(ivec2 texelPos, float depth) {
    return vec4(shadow_getTexCoordFromTexelPos(texelPos) * 2.f - 1.f, depth, 1.f);
}
