#pragma once
#include "inc/glsl.hpp"

const uint low8_mask  = 0b0000000011111111;
const uint high8_mask = 0b1111111100000000;

uint low8(uint ID) { return (ID & low8_mask); }
uint high8(uint ID) { return (ID & high8_mask) >> 8; }

struct GBufferData {
    vec4 rawData;

    // (3 + 1) * 8 位精度, 丢弃 diffuse.a, 放入 blockID 的高 8 位
    vec3 diffuse;
    uint _blockIDHigh8;

    // (3 + 1) * 8 位精度
    vec3  normal;
    float smoothness;

    // (3 + 1) * 8 位精度
    vec3  tangent;
    float metalness;

    // blockID 的低 8 位
    uint _blockIDLow8;

    uint blockID;
};

// 快速获取法线而不解开所有数据
vec3 gBufferNormal(vec4 rawData) {
    return unpackSnorm4x8(floatBitsToUint(rawData).y).xyz;
}

void packGBufferData(inout GBufferData gd) {
    gd._blockIDHigh8 = high8(gd.blockID);
    gd._blockIDLow8  = low8(gd.blockID);
    uint a           = packUnorm4x8(vec4(gd.diffuse, float(gd._blockIDHigh8) / 256));
    uint b           = packSnorm4x8(vec4(gd.normal, gd.smoothness));
    uint c           = packSnorm4x8(vec4(gd.tangent, gd.metalness));
    uint d           = gd._blockIDLow8;
    gd.rawData.xyzw  = uintBitsToFloat(uvec4(a, b, c, d));
}

void unpackGBufferData(inout GBufferData gd) {
    uvec4 abcd = floatBitsToUint(gd.rawData.xyzw);

    gd.diffuse       = unpackUnorm4x8(abcd.x).xyz;
    gd._blockIDHigh8 = uint(unpackUnorm4x8(abcd.x).w * 256);

    gd.normal     = unpackSnorm4x8(abcd.y).xyz;
    gd.smoothness = unpackSnorm4x8(abcd.y).w;

    gd.tangent   = unpackSnorm4x8(abcd.z).xyz;
    gd.metalness = unpackSnorm4x8(abcd.z).w;

    gd._blockIDLow8 = abcd.w;
    gd.blockID      = gd._blockIDLow8 | (gd._blockIDHigh8 << 8);
}
