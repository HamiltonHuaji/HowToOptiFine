#pragma once

// 光线反射次数上限
#define MAX_LIGHT_BOUNCE 3 // [1 2 3 4 5]

// sample per pixel
#define SAMPLE_PER_PIXEL 1 // [1 2 4 8 16 32 64]

// 单条光线步进的最大长度
#define MAX_RAY_LENGTH 24

// 光源亮度乘数
#define BRIGHTNESS 4 // [1 2 4 8 16 32]

// Temporal accumulation 时的累积系数, 越小越好, 但对光源变化的响应减弱
#define ALPHA 0.05 // [0.01 0.05 0.1 0.2]
#define ALPHA_MOMENTS 0.2

// A-trous 滤波次数
#define ATROUS_FILTER_PASS 6 // [0 1 2 3 4 5 6]

// 混合模式
// 0: 仅漫反射色
// 1: 仅光照
// 2: 光照 * 漫反射色
// 3: 色阶映射
#define COMPOSITE_OPTION 2 // [0 1 2 3]

// Gamma 校正值
#define GAMMA 1.8          // [0.5 1.0 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 2.5 3.0 3.5 4.0]
#define GAMMA_CORRECTION 0 // [0 1]

// 用于显示 About 信息
#define ABOUT 0 // [0 1 2 3 4 5 6]

// SVGF 参数
#define SIGMA_Z 1   // [0 1 2 4 8 16]
#define SIGMA_N 128 // [16 32 64 128 256 512]
#define SIGMA_L 32  // [1 2 4 6 8 10 12 16 32 64]

// A-trous 滤波器核
const float atrousKernel[3] = {1.0, 2.0 / 3.0, 1.0 / 6.0};

// 高斯卷积核
const float gaussKernel3[2][2] = {
    {1.0 / 4.0, 1.0 / 8.0},
    {1.0 / 8.0, 1.0 / 16.0}};
// 高斯卷积核
const float gaussKernel7[4][4] = {
    {1.0000, 1.0905, 1.4142, 2.1810},
    {1.0905, 1.1892, 1.5422, 2.3784},
    {1.4142, 1.5422, 2.0000, 3.0844},
    {2.1810, 2.3784, 3.0844, 4.7568}};

// 阴影贴图分辨率
const int shadowMapResolution = 8192;
// 可投下阴影的物体的最大距离
const float shadowDistance     = 128;

// 来自 SEUS PTGI
const float sunPathRotation    = -40.0f;
const int   superSamplingLevel = 0;

#define SHADOW_MAP_BIAS 0.9 // [0.0 0.6 0.8 0.9 0.95 0.99]

#define moment1 r
#define moment2 g
#define moments rg
#define history_length b

#define DISABLE_SHADOWMAP_BLUR true // [true false]
