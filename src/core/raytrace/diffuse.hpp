#pragma once

#include "inc/glsl.hpp"

#include "inc/random.hpp"
#include "inc/voxelmarch.hpp"

// voxelPos: primary ray 与场景的交点, 这个交点利用光栅化管线可以简单得到(即采样世界坐标系贴图, 或根据深度图直接算)
// tbn: TBN矩阵, 即由 tangent, binormal, normal 三个正交向量拼出的正交阵
// seed: 随机种子, 使用 uvec4(texelPos.xy, frameCounter, 0) 填充
// 结果分为直接光照 LDE 部分和间接光照 L(D|T)*E部分
// 并假设光源不受光照影响(反正也不太看得出来, 不太物理×3)
Illuminance raytrace_diffuse(vec3 voxelPos, mat3 tbn, uvec4 seed) {
    Ray r = Ray(voxelPos + exp2(-12) * tbn[2], tbn * uniform2dToHemisphere(random2d(seed)), vec4(1.));

    Illuminance illuminance = Illuminance(vec4(0), vec4(0));

    IntersectionData itd = intersect(r);
    // 计算直接光照
    if (!itd.isHit) {
        // 在有天空计算后应当在此时返回背景色
        return illuminance;
    }
    if (itd.isEmissive) {
        // 求交到光源, 为了使最大求交距离的边缘不太明显, 乘以一个下降因子
        illuminance.direct = itd.diffuse * float(BRIGHTNESS) * lightDropdown(itd.t);
        return illuminance;
    } else if (itd.isTranslucent) {
        // 求交到透明物, 暂时进行不太物理的处理, 即假设没有反射光.
        // 光线继续前进, 但给光线乘上当前方块的颜色(而非乘上与方块内行进路线长度相关的量, 不太物理×2)
        // 多向方块内行进一个小距离, 使新的光线起点落在方块内部
        r.ori = r.ori + r.dir * itd.t - exp2(-12) * itd.normal;
        r.product *= itd.diffuse;
    } else if (itd.diffuse.a < .0125) {
        // 求交到树叶等材质的镂空部分
        // 光线同样继续前进
        r.ori = r.ori + r.dir * itd.t - exp2(-12) * itd.normal;
    } else {
        // 求交到材质的不透明部分, 由于考虑的是 L(D|T)*E 的路径, 直接生成随机方向的反射光
        r.ori = r.ori + itd.t * r.dir + exp2(-12) * itd.normal;
        r.dir = generateDummyTBN(itd.normal) * uniform2dToHemisphere(random2d(seed + uvec4(0, 0, 0, 1)));
    }

    // 计算间接光照
    for (int bounce = 2; bounce <= MAX_LIGHT_BOUNCE; bounce++) {
        itd = intersect(r);
        if (itd.isHit) {
            if (itd.isEmissive) {
                r.product *= itd.diffuse * float(BRIGHTNESS) * lightDropdown(itd.t);
                break;
            } else if (bounce < MAX_LIGHT_BOUNCE) {
                if (itd.isTranslucent) {
                    r.ori = r.ori + r.dir * itd.t - exp2(-12) * itd.normal;
                    r.product *= itd.diffuse;
                } else if (itd.diffuse.a < .0125) {
                    r.ori = r.ori + r.dir * itd.t - exp2(-12) * itd.normal;
                } else {
                    r.ori = r.ori + r.dir * itd.t + exp2(-12) * itd.normal;
                    r.dir = generateDummyTBN(itd.normal) * uniform2dToHemisphere(random2d(seed + uvec4(0, 0, 0, 1)));
                    r.product *= itd.diffuse * lightDropdown(itd.t);
                }
            } else {
                // 没有下次求交了, 但还没碰到光源, 只能丢弃结果了
                r.product = vec4(0.);
            }
        } else {
            // 其实应该采样天空
            r.product = vec4(0.);
            break;
        }
    }
    illuminance.indirect = r.product;
    return illuminance;
}
