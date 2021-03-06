#pragma once

#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/utils.hpp"

vec3 shadowSpacePosition(vec3 localPos) {
    vec4 shadowPosition = shadowProjection * shadowModelView * vec4(localPos, 1.f);
    shadowPosition /= shadowPosition.w;
    return shadowPosition.xyz;
}

// 标准 gradient shadow map 实现
// 用法: shadowColor(localPos) * diffuseColor
vec3 shadowColor(vec3 localPos, vec3 worldNormal) {
    vec3 shadowLightDirection = normalize(shadowModelViewInverse[2].xyz);

    float nDotL = dot(shadowLightDirection, normalize(worldNormal));
    if (nDotL <= 1e-6) {
        return vec3(0);
    }

    vec3  shadowPosition = shadowSpacePosition(localPos);
    float dist           = length(shadowPosition.xy);
    float distortFactor  = (1.0 - SHADOW_MAP_BIAS) + dist * SHADOW_MAP_BIAS;
    shadowPosition.xy /= distortFactor;

    if (abs(shadowPosition.x) >= .999 || abs(shadowPosition.y) >= .999 || length(localPos) >= shadowDistance) {
        return KelvinToRGB(sunTemperature) * sunIntensity;
    }
    vec2  shadowCoord = shadowPosition.xy * .125f + .875f;
    float d           = shadowPosition.z * .5 + .5;

    vec4 shadowData = texture(tex_shadowcolor, shadowCoord);

    float z  = shadowData.r;
    float dz = shadowData.b;

    if (d >= z + dz * 4) {
        return vec3(0);
    }
    return KelvinToRGB(sunTemperature) * sunIntensity;
}

#include "inc/random.hpp"

// PCSS
// 用法: shadowColor(localPos) * diffuseColor
vec3 shadowColorFine(vec3 localPos, vec3 worldNormal, ivec2 texelPos, vec3 albedo) {
    // 指向光源
    vec3 shadowLightDirection = normalize(shadowModelViewInverse[2].xyz);

    float nDotL = dot(shadowLightDirection, normalize(worldNormal));
    if (nDotL <= 1e-6) {
        return vec3(0);
        // SSS
        // return shadowColor(localPos - (worldNormal + shadowLightDirection) * exp2(-8), -worldNormal) * (1.0 - (normalize(albedo.rgb + 0.000001) * 0.3));
    }

    vec3  shadowPosition = shadowSpacePosition(localPos);
    float dist           = length(shadowPosition.xy);
    float distortFactor  = (1.0 - SHADOW_MAP_BIAS) + dist * SHADOW_MAP_BIAS;
    shadowPosition.xy /= distortFactor;

    if (abs(shadowPosition.x) >= .999 || abs(shadowPosition.y) >= .999 || length(localPos) >= shadowDistance) {
        return KelvinToRGB(sunTemperature) * sunIntensity;
    }
    vec2  shadowCoord = shadowPosition.xy * .125f + .875f;
    float d           = shadowPosition.z * .5 + .5;

    vec4 shadowData = texture(tex_shadowcolor, shadowCoord);

    float z  = shadowData.r;
    float dz = shadowData.b;

    if (d >= z + dz * 4) {
        return vec3(0);
    }
    //  else if (d >= z - dz * 4) {
    //     // 屏幕空间阴影
    //     // 追踪一个很短的距离即可
    //     vec4 origin = gbufferProjection * gbufferModelView * vec4(localPos, 1.f);
    //     origin /= origin.w;
    //     for (int i=0; i<8; i++) {
    //         vec4 p = gbufferProjection * gbufferModelView * vec4(localPos + shadowLightDirection * (float(i) / 128.f), 1.f);
    //         p /= p.w;
    //         if (p.z > texelFetch(tex_gbuffer_depth, ivec2(viewSize * (p.xy * .5f + .5f)), 0).r) {
    //             return vec3(1,0,0);
    //         }
    //     }
    // }
    return KelvinToRGB(sunTemperature) * sunIntensity;
}
