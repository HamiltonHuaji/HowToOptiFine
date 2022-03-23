#pragma once
#include "inc/glsl.hpp"

const uint low8_mask  = 0b0000000011111111;
const uint high8_mask = 0b1111111100000000;

uint low8(uint ID) { return (ID & low8_mask); }
uint high8(uint ID) { return (ID & high8_mask) >> 8; }

struct GBufferData {
    vec4 rawData;

    // (3 + 1) * 8 位精度, 丢弃 diffuse.a
    vec3  diffuse;
    float smoothness;

    // (3 + 1) * 8 位精度
    vec3  normal;
    float metalness;

    // (3 + 1) * 8 位精度
    vec3  tangent;
    float emissivity;

    // 16 位精度
    uint blockID;
};

// 快速获取法线而不解开所有数据
vec3 gBufferNormal(vec4 rawData) {
    return unpackSnorm4x8(floatBitsToUint(rawData).y).xyz;
}

void packGBufferData(inout GBufferData gd) {
    uint a          = packUnorm4x8(vec4(gd.diffuse, gd.smoothness));
    uint b          = packSnorm4x8(vec4(gd.normal, gd.metalness));
    uint c          = packSnorm4x8(vec4(gd.tangent, gd.emissivity));
    uint d          = packUnorm2x16(vec2(gd.blockID) / 65535.0f);
    gd.rawData.xyzw = uintBitsToFloat(uvec4(a, b, c, d));
}

void unpackGBufferData(inout GBufferData gd) {
    uvec4 abcd = floatBitsToUint(gd.rawData.xyzw);

    gd.diffuse    = unpackUnorm4x8(abcd.x).xyz;
    gd.smoothness = unpackUnorm4x8(abcd.x).w;

    gd.normal    = unpackSnorm4x8(abcd.y).xyz;
    gd.metalness = unpackSnorm4x8(abcd.y).w;

    gd.tangent    = unpackSnorm4x8(abcd.z).xyz;
    gd.emissivity = unpackSnorm4x8(abcd.z).w;

    gd.blockID = uint(unpackUnorm2x16(abcd.w).x * 65535);
}
