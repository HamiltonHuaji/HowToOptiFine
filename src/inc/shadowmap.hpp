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
    vec2  shadowCoord = shadowPosition.xy * .25f + .75f;
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
vec3 shadowColorNice(vec3 localPos, vec3 worldNormal, ivec2 texelPos, vec3 albedo) {
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
    vec2  shadowCoord = shadowPosition.xy * .25f + .75f;
    float d           = shadowPosition.z * .5 + .5;

















    vec4 shadowData = texture(tex_shadowcolor, shadowCoord);

    float z  = shadowData.r;
    float dz = shadowData.b;

    if (d >= z + dz * 4) {
        return vec3(0);
    }
    return KelvinToRGB(sunTemperature) * sunIntensity;
}
