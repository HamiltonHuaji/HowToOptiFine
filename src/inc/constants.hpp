#pragma once

// 光线反射次数上限
#define MAX_LIGHT_BOUNCE 3 // [2 3 4 5]

// sample per pixel
#define SAMPLE_PER_PIXEL 8 // [1 2 4 8 16 32 64]

// 单条光线步进的最大长度
#define MAX_RAY_LENGTH 24

// 光源亮度乘数
#define BRIGHTNESS 4 // [1 2 4 8 16 32]

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

// A-trous 滤波器核
const float atrousKernel[5][5] = float[5][5](
    float[5](1.0 / 256.0, 1.0 / 64.0, 3.0 / 128.0, 1.0 / 64.0, 1.0 / 256.0),
    float[5](1.0 / 64.0, 1.0 / 16.0, 3.0 / 32.0, 1.0 / 16.0, 1.0 / 64.0),
    float[5](3.0 / 128.0, 3.0 / 32.0, 9.0 / 64.0, 3.0 / 32.0, 3.0 / 128.0),
    float[5](1.0 / 64.0, 1.0 / 16.0, 3.0 / 32.0, 1.0 / 16.0, 1.0 / 64.0),
    float[5](1.0 / 256.0, 1.0 / 64.0, 3.0 / 128.0, 1.0 / 64.0, 1.0 / 256.0));

// 高斯卷积核
const float gaussKernel[3][3] = float[3][3](
    float[3](1.0 / 16.0, 1.0 / 8.0, 1.0 / 16.0),
    float[3](1.0 / 8.0, 1.0 / 4.0, 1.0 / 8.0),
    float[3](1.0 / 16.0, 1.0 / 8.0, 1.0 / 16.0));

// 阴影贴图分辨率
const int shadowMapResolution = 8192;
// 可投下阴影的物体的最大距离
const float shadowDistance = 128;