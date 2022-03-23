#include "inc/buffers.hpp"
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

#include "inc/random.hpp"
vec2 TAAJitter = vec2(pcg3d(uvec3(frameCounter, texelPos)).xy) * (1.f / float(0xffffffffu)) - .5f;

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
    vec3 motionVector = texture(tex_motion_vector, texCoord).xyz;
    vec2 prevTexCoord = texCoord + motionVector.xy * .5f;
    return prevTexCoord;
}

// TAA, 重投影光照部分; 不更新颜色历史, 只更新二阶矩的历史和有效历史长度
#pragma rendertargets(5, 6)
void main() {
    float alpha = 0.1f;

    float depth = texelFetch(tex_gbuffer_depth, texelPos, 0).r;
    if (depth >= 1.0) {
        gl_FragData[0] = vec4(0, 0, 0, 0);
        gl_FragData[1] = vec4(0, 0, 0, 0);
        gl_FragData[2] = vec4(0, 0, 0, 0);
        return; // 天空
    }
    vec2  prevTexCoord       = getPrevTexCoord() + TAAJitter / viewSize;
    vec4  rawCurrGBufferData = texelFetch(tex_gbuffer, texelPos, 0);
    vec4  currWorldPos       = texelFetch(tex_worldpos, texelPos, 0);
    vec4  rawPrevGBufferData = texelFetch(tex_gbuffer_history, getTexelPosFromTexCoord(ivec2(viewSize), prevTexCoord), 0);
    vec4  prevWorldPos       = texelFetch(tex_worldpos_history, getTexelPosFromTexCoord(ivec2(viewSize), prevTexCoord), 0);
    bool  isVisible          = isReprojValid(prevTexCoord, rawCurrGBufferData, currWorldPos, rawPrevGBufferData, prevWorldPos);
    ivec2 prevTexelPos       = getTexelPosFromTexCoord(ivec2(viewSize), prevTexCoord);

    vec4 history   = texelFetch(tex_moments_history, texelPos, 0);
    vec4 nowDirect = texelFetch(tex_diffuse_direct, texelPos, 0);
    vec4 oldDirect = texture(tex_diffuse_direct_history, prevTexCoord);

    if (isVisible) {
        history.b += 1.;
    } else {
        alpha     = 1.f;
        history.b = 1;
    }
    if (any(isnan(oldDirect))) {
        alpha     = 1.0;
        history.b = 1;
    }

    if (alpha > 0) {
        alpha = max(alpha, 1.f / history.b);
    }

    // Accumulate color and moment2 here.
    // integratedColor = mix(old.color, now.color, alpha);
    // integratedMoment2 = mix(old.moment, now.moment, alpha);

    float luminanceDirect = luminance(nowDirect.rgb);
    oldDirect.a           = history.r;
    nowDirect.a           = luminanceDirect * luminanceDirect;

    vec4 outDirect = mix(oldDirect, nowDirect, alpha);

    history.r = outDirect.a;

    // Calculate variance here.
    if ((history.b < 4) || (alpha > .99f)) {
        outDirect.a = 1;
    } else {
        float luminanceOutDirect = luminance(outDirect.rgb);
        outDirect.a -= luminanceOutDirect * luminanceOutDirect;
    }

    gl_FragData[0] = history;
    gl_FragData[1] = outDirect;
}