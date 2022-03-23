#pragma once
#include "inc/glsl.hpp"

#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/uniforms.hpp"
#include "inc/voxelspace.hpp"

////////////////////////////////////////////////////////////////////
///  _____                            _              _   _ _ _   ///
/// |_   _|                          | |            | | | | | |  ///
///   | |  _ __ ___  _ __   ___  _ __| |_ __ _ _ __ | |_| | | |  ///
///   | | | '_ ` _ \| '_ \ / _ \| '__| __/ _` | '_ \| __| | | |  ///
///  _| |_| | | | | | |_) | (_) | |  | || (_| | | | | |_|_|_|_|  ///
/// |_____|_| |_| |_| .__/ \___/|_|   \__\__,_|_| |_|\__(_|_|_)  ///
///                 | |                                          ///
///                 |_|                                          ///
///        重要: 此处不能声明 uniform sampler2D tex_voxel;          ///
///        否则 OptiFine 会不创建这块缓冲区, 读不到体素数据            ///
////////////////////////////////////////////////////////////////////
uniform sampler2D shadowcolor0;
uniform sampler2D shadowtex0;
uniform sampler2D tex_atlas_diffuse;
uniform sampler2D tex_atlas_normal;
uniform sampler2D tex_atlas_specular;

const vec3 xmask = vec3(1, 0, 0);
const vec3 ymask = vec3(0, 1, 0);
const vec3 zmask = vec3(0, 0, 1);

// 体素空间的光线
// pos = ori + t * dir
struct Ray {
    vec3 ori;
    vec3 dir;
    // 沿着光线路径遇到的 BRDF 和余弦投影之连乘积
    vec4 product;
};
struct Illuminance {
    vec4 direct;
    vec4 indirect;
};

Ray emitRayFromVoxelPos(vec3 origin, vec3 dir) {
    return Ray(origin, normalize(dir), vec4(1.f));
}
Ray emitRayFromLocalPos(vec3 origin, vec3 dir) {
    return emitRayFromVoxelPos(getVoxelPosFromLocalPos(origin), dir);
}
Ray emitRayFromWorldPos(vec3 origin, vec3 dir) {
    return emitRayFromLocalPos(origin - cameraPosition, dir);
}

vec4 sampleVoxelColor(ivec2 texelPos) {
    return texelFetch(tex_voxel, texelPos, 0);
}
float sampleVoxelDepth(ivec2 texelPos) {
    return texelFetch(tex_voxel_depth, texelPos, 0).r;
}
VoxelData sampleVoxelData(ivec2 texelPos) {
    VoxelData vd;
    vd.rawData = sampleVoxelColor(texelPos);
    return vd;
}

// 包含了体素数据的求交结果
struct IntersectionData {

    // 路径长度
    float t;

    // 是否命中
    bool isHit;

    // 是否发光
    bool isEmissive;

    // 是否半透明(即需同时处理反射和透射)
    // 对于镂空而不透明的材质, 该值仍然为 false, 应检测 diffuse.a 来决定是否要穿过方块进行追踪
    bool isTranslucent;

    // 被命中方块的体素坐标
    ivec3 voxelPos;

    // 命中点的漫反射色
    vec4 diffuse;
    // 命中点的 specular 采样结果
    // r 分量为 smoothness
    // g 分量为 metalness
    // b 分量为 emissivity
    vec3 specular;

    // 命中点的法线
    // T 和 B 没保存, 暂时瞎生成一个 TBN
    vec3 normal;

    // 方块种类
    uint blockID;
};
IntersectionData makeIntersectionData() {
    return IntersectionData(
        0.f,
        false,
        false,
        false,
        ivec3(0.),
        vec4(0.),
        vec3(0.),
        vec3(0.),
        0);
}

int blockIntersect(Ray r, out float t, out VoxelData vd) {
    t            = 0; // important!!!
    vec3  invdir = 1.f / max(abs(r.dir), vec3(exp2(-12)));
    ivec3 rsign  = ivec3(max(sign(r.dir), vec3(0)));

    vec3  hitPos   = r.ori;
    ivec3 voxelPos = ivec3(floor(hitPos));
    vec3  hitDist  = abs(vec3(voxelPos + rsign) - hitPos) * invdir;
    for (int i = 0; i < MAX_RAY_LENGTH; i++) {
        t += min(hitDist.x, min(hitDist.y, hitDist.z));
        hitPos   = r.ori + (t + exp2(-12)) * r.dir;
        voxelPos = ivec3(floor(hitPos));
        vd       = sampleVoxelData(getTexelPosFromVoxelPos(voxelPos));
        if (vd.rawData.a > 0) {
            unpackVoxelData(vd);
            if (!isAir(vd.blockID)) {
                return i;
            }
        }
        hitDist = abs(vec3(voxelPos + rsign) - (r.ori + t * r.dir)) * invdir;
    }
    t = -1;
    return -1;
}

float lightDropdown(float t) {
    if (t <= 16) { return 1; }
    if (t >= MAX_RAY_LENGTH) { return 0; }
    return (t - 16) / (MAX_RAY_LENGTH - 16);
}

IntersectionData intersect(in Ray r) {
    VoxelData        vd;
    IntersectionData itd;
    itd.t = 0;
    if (blockIntersect(r, itd.t, vd) < 0) {
        itd.isHit = false;
        return itd;
    } else {
        itd.isHit = true;
    }

    vec3 ahitPos = floor(r.ori + (itd.t - exp2(-12)) * r.dir);
    vec3 bhitPos = floor(r.ori + (itd.t + exp2(-12)) * r.dir);
    vec3 mask    = normalize(sign(abs(ahitPos - bhitPos)));
    itd.normal   = sign(-r.dir) * mask;

    vec3 hitPos    = r.ori + itd.t * r.dir;
    itd.voxelPos   = ivec3(floor(hitPos - itd.normal * exp2(-12)));
    ivec2 texelPos = getTexelPosFromVoxelPos(itd.voxelPos);

    itd.isTranslucent          = isTranslucent(vd.blockID);
    itd.isEmissive             = isEmissive(vd.blockID);
    vec3 fhitPos               = fract(hitPos - itd.normal * exp2(-12));
    vec2 relativeTexCoord      = fhitPos.yz * mask.x + fhitPos.zx * mask.y + fhitPos.xy * mask.z; // 还需要加上缩放和偏移才能得到真正的纹理坐标
    vec2 absoluteTexelPosBase  = vd.midTexCoord - (vd.spriteSize / 2.f);
    vec2 absoluteTexelPosScale = vd.spriteSize;
    vec2 absoluteTexelPos      = absoluteTexelPosBase + absoluteTexelPosScale * relativeTexCoord;

    itd.diffuse  = vec4(vd.color, 1.f) * (texture(tex_atlas_diffuse, absoluteTexelPos));
    itd.specular = texture(tex_atlas_specular, absoluteTexelPos).rgb;
    itd.blockID  = vd.blockID;
    return itd;
}

const mat3 n2t = mat3(0, 0, 1,
                      1, 0, 0,
                      0, 1, 0);

// https://graphics.pixar.com/library/OrthonormalB/paper.pdf
// float sign = copysignf(1.0f, n.z);
// const float a = -1.0f / (sign + n.z);
// const float b = n.x * n.y * a;
// vec3 b1 = vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
// vec3 b2 = vec3(b, sign + n.y * n.y * a, -n.y);

mat3 generateDummyTBN(vec3 normal) {
    vec3 tangent  = n2t * step(normal, vec3(.5f));
    tangent       = normalize(tangent - normal * dot(tangent, normal));
    vec3 binormal = normalize(cross(tangent, normal));
    return mat3(tangent, binormal, normal);
}
