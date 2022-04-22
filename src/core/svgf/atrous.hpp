#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_gbuffer_depth;
uniform sampler2D tex_localpos;
uniform sampler2D tex_diffuse_direct;

float computeVarianceCenter() {
    float sum = 0.f;

    const int radius = 1;
    for (int yy = -radius; yy <= radius; yy++) {
        for (int xx = -radius; xx <= radius; xx++) {
            ivec2       p = texelPos + ivec2(xx, yy);
            const float k = gaussKernel3[abs(xx)][abs(yy)];
            sum += texelFetch(tex_diffuse_direct, p, 0).a * k;
        }
    }
    return sum;
}
float linearDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}
float computeWeight(
    float depthCenter, float depthP, float phiDepth,
    vec3 normalCenter, vec3 normalP, float phiNormal,
    float luminanceIllumCenter, float luminanceIllumP, float phiIllum) {
    float weightNormal = pow(saturate(dot(normalCenter, normalP)), phiNormal);
    float weightZ      = (phiDepth == 0) ? 0.0f : abs(depthCenter - depthP) / phiDepth;
    float weightLillum = abs(luminanceIllumCenter - luminanceIllumP) / phiIllum;

    float weightIllum = exp(0.0 - max(weightLillum, 0.0) - max(weightZ, 0.0)) * weightNormal;
    return weightIllum;
}

#ifndef LEVEL
#define LEVEL 0
#endif
out vec4 outColor0;

#pragma rendertargets(6)
void main() {
    float epsVariance = 1e-10;

    float var = computeVarianceCenter();

    vec4 illumination = texelFetch(tex_diffuse_direct, texelPos, 0);
    if (any(isnan(illumination))) {
        illumination = vec4(0,0,0,1);
    }
    // if ((LEVEL >= ATROUS_FILTER_PASS) || (texCoord.x < .5f)){
    if (LEVEL >= ATROUS_FILTER_PASS){
        outColor0 = illumination;
        return;
    }
    float illuminationIllumCenter = luminance(illumination.rgb);

    vec3  normalCenter = gBufferNormal(texelFetch(tex_gbuffer, texelPos, 0));
    float depthCenter  = linearDepth(texture(tex_gbuffer_depth, texCoord).r);

    float q_gradz = max(abs(dFdx(depthCenter)), abs(dFdy(depthCenter)));

    float sumWIllumination = 1.0;
    vec4  sumIllumination  = illumination;

    int stepSize = 1 << LEVEL;

    float phiDepth  = SIGMA_Z * max(q_gradz, 1e-8) * stepSize;
    float phiNormal = SIGMA_N;
    float phiIllum  = SIGMA_L * sqrt(max(1e-8, var));

    for (int yy = -2; yy <= 2; yy++) {
        for (int xx = -2; xx <= 2; xx++) {
            ivec2 p      = texelPos + ivec2(xx, yy) * stepSize;
            bool  valid  = p.x >= 0 && p.y >= 0 && p.x < viewSize.x && p.y < viewSize.y;
            float kernel = atrousKernel[abs(xx)] * atrousKernel[abs(yy)];
            if (valid && (xx != 0 || yy != 0)) {
                // skip center pixel, it is already accumulated
                vec4  illuminationP      = texelFetch(tex_diffuse_direct, p, 0);
                float illuminationIllumP = luminance(illuminationP.rgb);
                vec3  normalP            = gBufferNormal(texelFetch(tex_gbuffer, p, 0));
                float depthP             = linearDepth(texture(tex_gbuffer_depth, getTexCoordFromTexelPos(ivec2(viewSize), p)).r);

                // compute the edge-stopping functions
                float w = computeWeight(
                    depthCenter, depthP, phiDepth * length(vec2(xx, yy)),
                    normalCenter, normalP, phiNormal,
                    illuminationIllumCenter, illuminationIllumP, phiIllum);

                float wIllumination = w * kernel;

                // alpha channel contains the variance, therefore the weights need to be squared, see paper for the formula
                sumWIllumination += wIllumination;
                sumIllumination += vec4(vec3(wIllumination), wIllumination * wIllumination) * illuminationP;
            }
        }
    }
    vec4 filteredIllumination = vec4(sumIllumination / vec4(vec3(sumWIllumination), sumWIllumination * sumWIllumination));
    outColor0            = filteredIllumination;
}
