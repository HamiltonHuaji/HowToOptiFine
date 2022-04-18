#pragma once

#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

vec3 shadowSpacePosition(vec3 localPos) {
    vec4 shadowPosition = shadowProjection * shadowModelView * vec4(localPos, 1.f);
    shadowPosition /= shadowPosition.w;
    return shadowPosition.xyz;
}

// ESM
// 用法: shadowColor(localPos) * diffuseColor
vec3 shadowColor(vec3 localPos, vec3 worldNormal) {
    vec3 lightDir = normalize(shadowLightPosition);
    if (dot(lightDir, upPosition) < 0) {
        lightDir = -lightDir;
    }

    vec3  shadowPosition = shadowSpacePosition(localPos);
    float dist           = length(shadowPosition.xy);
    float distortFactor  = (1.0 - SHADOW_MAP_BIAS) + dist * SHADOW_MAP_BIAS;
    shadowPosition.xy /= distortFactor;
    if (abs(shadowPosition.x) >= 1 || abs(shadowPosition.y) >= 1) {
        // 阴影贴图以外
        return KelvinToRGB(sunTemperature) * sunIntensity * max(0.1f, -dot(worldNormal, lightDir));
    }
    // 阴影贴图的右上角
    vec2 shadowCoord = shadowPosition.xy * .25f + .75f;

#if 0
    vec4      shadowData = vec4(0);
    const int radius     = 1;
    for (int yy = -radius; yy <= radius; yy++) {
        for (int xx = -radius; xx <= radius; xx++) {
            const float k = gaussKernel[abs(xx)][abs(yy)];
            shadowData += texture(tex_shadowcolor, shadowCoord + vec2(xx, yy) / float(shadowMapResolution)) * k;
        }
    }
#else
    vec4 shadowData = texture(tex_shadowcolor, shadowCoord);
#endif
    float z  = shadowData.r;
    float z2 = shadowData.g;
    if (z > .999f) {
        return KelvinToRGB(sunTemperature) * sunIntensity * max(0.1f, -dot(worldNormal, lightDir));
    }
    float d = shadowPosition.z * .5 + .5;

    float f = 1;
    if (d >= z + .00001) {
        float sigma2 = z2 - z * z + 1e-6;
        float t      = d - z;
        float p_max  = sigma2 / (sigma2 + t * t);
        f            = max(saturate(p_max), 0);
    }

    return f * KelvinToRGB(sunTemperature) * sunIntensity * max(0.1f, dot(worldNormal, lightDir));
}
