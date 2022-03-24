#include "inc/buffers.hpp"
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

#include "inc/random.hpp"
// vec2 TAAJitter = vec2(pcg3d(uvec3(frameCounter, texelPos)).xy) * (1.f / float(0xffffffffu)) - .5f;

uniform sampler2D tex_gbuffer_depth;
uniform sampler2D tex_gbuffer;
uniform sampler2D tex_worldpos;
uniform sampler2D tex_gbuffer_history;
uniform sampler2D tex_worldpos_history;
uniform sampler2D tex_motion_vector;
uniform sampler2D tex_moments_history;
uniform sampler2D tex_diffuse_direct;
uniform sampler2D tex_diffuse_direct_history;

bool isReprojValid(vec2 prevTexCoord, vec4 rawCurrGBufferData, vec4 currWorldPos, vec4 rawPrevGBufferData, vec4 prevWorldPos) {
    vec2 prevCheck = abs(prevTexCoord - vec2(.5f));
    if (prevCheck.x > .5f || prevCheck.y > .5f) {
        return false;
    }
    if (distance(currWorldPos.xyz, prevWorldPos.xyz) > .1 || pow(dot(gBufferNormal(rawCurrGBufferData), gBufferNormal(rawPrevGBufferData)), 16) < .95) {
        return false;
    }
    return true;
}

vec2 getPrevTexCoord() {
    vec3 motionVector = texelFetch(tex_motion_vector, texelPos, 0).xyz;
    vec2 prevTexCoord = texCoord + motionVector.xy;
    return prevTexCoord;
}

bool loadPrevData(out vec4 prevIllum, out vec2 prevMoments, out float historyLength) {

    vec2 prevTexCoord = getPrevTexCoord();

    vec4  rawCurrGBufferData = texelFetch(tex_gbuffer, texelPos, 0);
    vec4  currWorldPos       = texelFetch(tex_worldpos, texelPos, 0);

    prevIllum   = vec4(0,0,0,0);
    prevMoments = vec2(0,0);

    bool tapValid[4];
    ivec2 tapOffset[4] = {ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1)};
    vec2 unnormPrevTexelCoord = prevTexCoord * viewSize - vec2(.5f);
    ivec2 prevTexelPos = ivec2(floor(unnormPrevTexelCoord));

    // check for all 4 taps of the bilinear filter for validity
    bool valid = false;

    // vec4  rawPrevGBufferData = texelFetch(tex_gbuffer_history, prevTexelPos, 0);
    // vec4  prevWorldPos       = texelFetch(tex_worldpos_history, prevTexelPos, 0);
    // valid = isReprojValid(prevTexCoord, rawCurrGBufferData, currWorldPos, rawPrevGBufferData, prevWorldPos);

    for (int sampleIdx = 0; sampleIdx < 4; sampleIdx++) {
        ivec2 loc = prevTexelPos + tapOffset[sampleIdx];
        vec4  rawPrevGBufferData = texelFetch(tex_gbuffer_history, loc, 0);
        vec4  prevWorldPos       = texelFetch(tex_worldpos_history, loc, 0);

        tapValid[sampleIdx] = isReprojValid(prevTexCoord, rawCurrGBufferData, currWorldPos, rawPrevGBufferData, prevWorldPos);
        valid = valid || tapValid[sampleIdx];
    }
    // if (valid) {
    //     prevIllum   = texelFetch(tex_diffuse_direct, prevTexelPos, 0);
    //     prevMoments = texelFetch(tex_moments_history, prevTexelPos, 0).xy;
    //     historyLength = texelFetch(tex_moments_history, prevTexelPos, 0).z;
    // } else {
    //     prevIllum   = vec4(0,0,0,0);
    //     prevMoments = vec2(0,0);
    //     historyLength = 0;
    // }
    // return valid;

    if (valid) {
        float sumw = 0;

        // vec2 fractPrevTexelPos = vec2(0);
        vec2 fractPrevTexelPos = fract(unnormPrevTexelCoord);

        /* clang-format off */
        // bilinear weights
        const float w[4] = { (1 - fractPrevTexelPos.x) * (1 - fractPrevTexelPos.y),
                                  fractPrevTexelPos.x  * (1 - fractPrevTexelPos.y),
                             (1 - fractPrevTexelPos.x) *      fractPrevTexelPos.y,
                                  fractPrevTexelPos.x  *      fractPrevTexelPos.y };
        // const float w[4] = { 1, 0, 0, 0 };
        /* clang-format on */

        // perform the actual bilinear interpolation
        for (int sampleIdx = 0; sampleIdx < 4; sampleIdx++) {
            ivec2 loc = prevTexelPos + tapOffset[sampleIdx];
            if (tapValid[sampleIdx]) {
                prevIllum   += w[sampleIdx] * texelFetch(tex_diffuse_direct_history, loc, 0);
                prevMoments += w[sampleIdx] * texelFetch(tex_moments_history, loc, 0).xy;
                sumw        += w[sampleIdx];
             }
        }

        // redistribute weights in case not all taps were used
        valid = (sumw >= 0.01);
        prevIllum   = valid ? prevIllum / sumw   : vec4(0, 0, 0, 0);
        prevMoments = valid ? prevMoments / sumw : vec2(0, 0);
    }

    // // perform cross-bilateral filter in the hope to find some suitable samples somewhere
    if (!valid) {
        float nValid = 0.0;
        // this code performs a binary descision for each tap of the cross-bilateral filter
        const int radius = 1;
        for (int yy = -radius; yy <= radius; yy++) {
            for (int xx = -radius; xx <= radius; xx++) {
                ivec2 p = prevTexelPos + ivec2(xx, yy);
                vec4  rawPrevGBufferData = texelFetch(tex_gbuffer_history, p, 0);
                vec4  prevWorldPos       = texelFetch(tex_worldpos_history, p, 0);

                if (isReprojValid(prevTexCoord, rawCurrGBufferData, currWorldPos, rawPrevGBufferData, prevWorldPos)) {
                    prevIllum   += texelFetch(tex_diffuse_direct_history, p, 0);
                    prevMoments += texelFetch(tex_moments_history, p, 0).xy;
                    nValid += 1.0;
                }
            }
        }
        if (nValid > 0) {
            valid = true;
            prevIllum   /= nValid;
            prevMoments /= nValid;
        }
    }

    if (valid) {
        historyLength = texelFetch(tex_moments_history, prevTexelPos, 0).z;
    }
    else {
        prevIllum   = vec4(0,0,0,0);
        prevMoments = vec2(0,0);
        historyLength = 0;
    }
    return valid;
}

// TAA, 重投影光照部分; 不更新颜色历史, 只更新二阶矩的历史和有效历史长度
#pragma rendertargets(5, 6, 15)
void main() {
    float Alpha = 0.1f;
    float AlphaMoments = 0.1f;

    float depth = texelFetch(tex_gbuffer_depth, texelPos, 0).r;
    if (depth >= 1.0) {
        gl_FragData[0] = vec4(0, 0, 0, 0);
        gl_FragData[1] = vec4(0, 0, 0, 0);
        gl_FragData[2] = vec4(0, 0, 0, 0);
        return; // 天空
    }

    vec4 illumination = texelFetch(tex_diffuse_direct, texelPos, 0);
    if (any(isnan(illumination))) {
        illumination = vec4(0);
    }
    float historyLength;
    vec4 prevIllumination;
    vec2 prevMoments;

    bool success = loadPrevData(prevIllumination, prevMoments, historyLength);
    historyLength = min(32.0f, success ? historyLength + 1.0f : 1.0f);

    float alpha        = success ? max(Alpha,        1.0 / historyLength) : 1.0;
    float alphaMoments = success ? max(AlphaMoments, 1.0 / historyLength) : 1.0;

    vec2 currMoments;
    currMoments.r = luminance(illumination.rgb);
    currMoments.g = currMoments.r * currMoments.r;

    vec2 outMoments = mix(prevMoments, currMoments, alphaMoments);
    vec4 outIllumination = mix(prevIllumination, illumination, alpha);
    float variance = max(0, outMoments.g - outMoments.r * outMoments.r);
    outIllumination.a = variance;

    // tex_moments_history.x: 1阶矩, 即 luminance(inputColor) 的累计
    // tex_moments_history.y: 2阶矩, 即 pow(luminance(inputColor), 2) 的累计
    // tex_moments_history.z: 有效历史长度
    vec4 outMomentsHistory = vec4(outMoments, historyLength, 1.f);

    gl_FragData[0] = outMomentsHistory;
    gl_FragData[1] = outIllumination;
    gl_FragData[2] = vec4(alpha, .1f * sqrt(variance) / max(1e-3, luminance(outIllumination.rgb)), success ? 1 : 0, 1);
}