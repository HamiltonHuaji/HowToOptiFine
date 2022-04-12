#pragma once
#include "inc/glsl.hpp"
uvec2 pcg2d(uvec2 v) {
    v = v * 1664525u + 1013904223u;

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v >> 16u);

    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;

    v = v ^ (v >> 16u);

    return v;
}
uvec3 pcg3d(uvec3 v) {

    v = v * 1664525u + 1013904223u;

    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;

    v = v ^ (v >> 16u);

    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;

    return v;
}
uvec4 pcg4d(uvec4 v) {
    v = v * 1664525u + 1013904223u;

    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;

    v = v ^ (v >> 16u);

    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;

    return v;
}

vec2 Martin_Roberts_R2(int n) {
    double g = 1.32471795724474602596;
    double a1 = 1.0/g;
    double a2 = 1.0/(g*g);
    return vec2(fract(dvec2(0.5+a1*n, 0.5+a2*n)));
}

vec2 random2d(uvec4 seed) {
    return vec2(pcg4d(seed).xy) * (1.f / float(0xffffffffu));
}

#define tex_stbn colortex10
uniform sampler3D tex_stbn;

vec3 stbnSampler(uvec2 texelPos, uint frameCounter) {
    vec3 s = texelFetch(tex_stbn, ivec3(frameCounter % 64, texelPos.x % 128, texelPos.y % 128), 0).xyz;
    return normalize(s * 2.f - 1.f);
}
