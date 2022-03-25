#pragma once

// 光线反射次数上限
#define MAX_LIGHT_BOUNCE 3 // [1 2 3 4 5]

// sample per pixel
#define SAMPLE_PER_PIXEL 8 // [1 2 4 8 16 32 64]

// 单条光线步进的最大长度
#define MAX_RAY_LENGTH 24

// 光源亮度乘数
#define BRIGHTNESS 4 // [1 2 4 8 16 32]

// Temporal accumulation 时的累积系数, 越小越好, 但对光源变化的响应减弱
#define ALPHA 0.05 // [0.01 0.05 0.1 0.2]
#define ALPHA_MOMENTS 0.2

// A-trous 滤波次数
#define ATROUS_FILTER_PASS 5 // [0 1 2 3 4 5]

// 混合模式
// 0: 仅显示直接光照
// 1: (直接光照 + 间接光照)
// 2: (直接光照 + 间接光照) * 漫反射
// 3: 色阶映射
#define COMPOSITE_OPTION 2 // [0 1 2 3]

// Gamma 校正值
#define GAMMA 1.8 // [0.5 1.0 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 2.5 3.0 3.5 4.0]
#define GAMMA_CORRECTION 0 // [0 1]

// 用于显示 About 信息
#define ABOUT 0 // [0 1 2 3 4 5 6]

// SVGF 参数
#define SIGMA_Z 1 // [0 1 2 4 8 16]
#define SIGMA_N 128 // [16 32 64 128 256 512]
#define SIGMA_L 10 // [1 2 4 6 8 10 12 16 32]

// A-trous 滤波器核
const float atrousKernel[3] = { 1.0, 2.0 / 3.0, 1.0 / 6.0 };

// 高斯卷积核
const float gaussKernel[2][2] = float[2][2](
    float[2](1.0 / 4.0, 1.0 / 8.0),
    float[2](1.0 / 8.0, 1.0 / 16.0));

// 阴影贴图分辨率
const int shadowMapResolution = 8192;
// 可投下阴影的物体的最大距离
const float shadowDistance = 128;

#define moment1 r
#define moment2 g
#define moments rg
#define history_length b
