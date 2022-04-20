#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer_depth;
uniform sampler2D tex_gbuffer;
uniform sampler2D tex_localpos;
uniform sampler2D tex_moments_history;
uniform sampler2D tex_diffuse_direct;

float linearDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}
float computeWeight(
    float depthCenter, float depthP, float phiDepth,
    vec3 normalCenter, vec3 normalP, float phiNormal,
    float luminanceIllumCenter, float luminanceIllumP, float phiIllum) {
    const float weightNormal = pow(saturate(dot(normalCenter, normalP)), phiNormal);
    const float weightZ      = (phiDepth == 0) ? 0.0f : abs(depthCenter - depthP) / phiDepth;
    const float weightLillum = abs(luminanceIllumCenter - luminanceIllumP) / phiIllum;

    const float weightIllum = exp(0.0 - max(weightLillum, 0.0) - max(weightZ, 0.0)) * weightNormal;

    return weightIllum;
}

out vec4 outColor0;

// 对于历史少于 4 的像素计算其空间的矩
#pragma rendertargets(6)
void main() {

    float h = max(1.0f, texelFetch(tex_moments_history, texelPos, 0).history_length);

    vec4 outColor = vec4(0);
    if (h < 4.0) { // not enough temporal history available
        float sumWIllumination = 0.0;
        vec3  sumIllumination  = vec3(0.0, 0.0, 0.0);
        vec2  sumMoments       = vec2(0.0, 0.0);

        vec4  illuminationCenter  = texelFetch(tex_diffuse_direct, texelPos, 0);
        float lIlluminationCenter = luminance(illuminationCenter.rgb);
        float depthCenter         = linearDepth(texture(tex_gbuffer_depth, texCoord).r);

        float q_gradz = max(abs(dFdx(depthCenter)), abs(dFdy(depthCenter)));
        if (depthCenter == 1) {
            // current pixel does not a valid depth => must be envmap => do nothing
            outColor0 = illuminationCenter;
            return;
        }
        vec3  normalCenter     = gBufferNormal(texelFetch(tex_gbuffer, texelPos, 0));
        float phiLIllumination = SIGMA_L;
        float phiDepth         = SIGMA_Z * max(q_gradz, 1e-6);
        float phiNormal        = SIGMA_N;

        // compute first and second moment spatially. This code also applies cross-bilateral
        // filtering on the input illumination.
        int radius = 3;

        for (int yy = -radius; yy <= radius; yy++) {
            for (int xx = -radius; xx <= radius; xx++) {
                ivec2 p         = texelPos + ivec2(xx, yy);
                bool  valid     = p.x >= 0 && p.y >= 0 && p.x < viewSize.x && p.y < viewSize.y;
                bool  samePixel = (xx == 0 && yy == 0);
                float kernel    = 1.0;

                if (valid) {
                    vec3  illuminationP  = texelFetch(tex_diffuse_direct, p, 0).rgb;
                    vec2  momentsP       = texelFetch(tex_moments_history, p, 0).moments;
                    float lIlluminationP = luminance(illuminationP.rgb);
                    float depthP         = linearDepth(texture(tex_gbuffer_depth, getTexCoordFromTexelPos(ivec2(viewSize), p)).r);
                    vec3  normalP        = gBufferNormal(texelFetch(tex_gbuffer, p, 0));

                    float w = computeWeight(
                        depthCenter, depthP, phiDepth * length(vec2(xx, yy)),
                        normalCenter, normalP, phiNormal,
                        lIlluminationCenter, lIlluminationP, phiLIllumination);

                    sumWIllumination += w;
                    sumIllumination += illuminationP * w;
                    sumMoments += momentsP * w;
                }
            }
        }

        // Clamp sum to >0 to avoid NaNs.
        sumWIllumination = max(sumWIllumination, 1e-6f);

        sumIllumination /= sumWIllumination;
        sumMoments /= sumWIllumination;

        // compute variance using the first and second moments
        float variance = sumMoments.g - sumMoments.r * sumMoments.r;

        // give the variance a boost for the first frames
        variance *= (4.0 / h);
        outColor = vec4(sumIllumination, variance);
    } else {
        // do nothing, pass data unmodified
        outColor = texelFetch(tex_diffuse_direct, texelPos, 0);
    }
    outColor0 = outColor;
}