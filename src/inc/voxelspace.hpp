#pragma once

#include "inc/blockmapping.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"

const int voxelR = 128;
const int voxelD = 2 * voxelR;
const int voxelA = voxelD * voxelD;
const int voxelV = voxelD * voxelD * voxelD;

ivec2 getTexelPosFromVoxelPos(ivec3 voxelPos) {
    uvec3 uvoxelPos = clamp(uvec3(voxelPos + ivec3(voxelR, 0, voxelR)), uvec3(0), uvec3(voxelD - 1));
    return ivec2(uvoxelPos.xz + uvec2(mod(uvoxelPos.y, 16) * voxelD, floor(float(uvoxelPos.y) / 16.f) * voxelD));
}

vec3 getVoxelPosFromLocalPos(vec3 localPos) {
    return localPos + cameraPosition.xyz - floor(cameraPosition.xyz) * vec3(1, 0, 1);
}
ivec3 getBlockPosFromLocalPos(vec3 localPos) {
    return ivec3(floor(getVoxelPosFromLocalPos(localPos)));
}

#include "inc/utils.hpp"

struct VoxelData {
    vec4 rawData;

    // 纹理坐标均为 texCoord

    // 2 * 16 位精度
    vec2 midTexCoord;

    // 2 * 16 位精度
    vec2 spriteSize;

    // 3 * 8 位精度
    vec3 color;

    // 16 位精度
    uint blockID;

    // 6 * 2 位精度, 但暂时不实现之
    int tbn;
};
VoxelData makeVoxelData() {
    return VoxelData(
        vec4(0.),
        vec2(0.),
        vec2(0.),
        vec3(0.),
        0,
        0);
}

void packVoxelData(inout VoxelData vd) {
    uint a          = packUnorm2x16(vd.midTexCoord);
    uint b          = packUnorm2x16(vd.spriteSize);
    uint c          = packUnorm4x8(vec4(vd.color, 1.f));

    uint d          = packUnorm2x16(vec2(vd.blockID) / 65535.0f);
    // ver 1, 不可用
    // vd.rawData.xyzw = uintBitsToFloat(uvec4(a, b, c, d));

    // ver 2, 可用
    vd.rawData.xyzw = vec4(uintBitsToFloat(uvec3(a, b, c)), float(vd.blockID) / 65535.0f);
}
void unpackVoxelData(inout VoxelData vd) {
    uvec4 abcd     = floatBitsToUint(vd.rawData.xyzw);
    vd.midTexCoord = unpackUnorm2x16(abcd.x);
    vd.spriteSize  = unpackUnorm2x16(abcd.y);
    vd.color       = unpackUnorm4x8(abcd.z).rgb;

    // ver 1
    // vd.blockID     = uint(float(unpackUnorm2x16(abcd.w).x) * 65535);
    // ver 2
    vd.blockID     = uint(vd.rawData.w * 65535);
}
