#include "inc/buffers.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/gbuffer.hpp"
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
uniform sampler2D tex_diffuse_indirect;
uniform sampler2D tex_diffuse_direct_history;
uniform sampler2D tex_diffuse_indirect_history;

float linearDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec2 getPrevTexCoord(out bool isVisible) {
    vec3 motionVector = texture(tex_motion_vector, texCoord).xyz;
    vec2 prevTexCoord = texCoord + motionVector.xy * .5f;
    vec3 worldPos     = texture(tex_worldpos, texCoord).xyz;
    vec3 prevWorldPos = texture(tex_worldpos_history, prevTexCoord).xyz;

    vec4 rawGBufferData     = texelFetch(tex_gbuffer, texelPos, 0);
    vec4 rawPrevGBufferData = texelFetch(tex_gbuffer_history, getTexelPosFromTexCoord(ivec2(viewSize), prevTexCoord), 0);

    if (length(worldPos - prevWorldPos) > .1 || dot(gBufferNormal(rawGBufferData), gBufferNormal(rawPrevGBufferData)) < .95) {
        isVisible = false;
    } else {
        isVisible = true;
    }
    return prevTexCoord;
}

// TAA, 重投影 direct 和 indirect 部分; 不更新颜色历史, 只更新二阶矩的历史和有效历史长度
#pragma rendertargets(5, 6, 7)
void main() {
    float alpha = 0.1f;

    float depth = texelFetch(tex_gbuffer_depth, texelPos, 0).r;
    if (depth >= 1.0) {
        gl_FragData[0] = vec4(0, 0, 0, 0);
        gl_FragData[1] = vec4(0, 0, 0, 0);
        gl_FragData[2] = vec4(0, 0, 0, 0);
        return; // 天空
    }
    bool  isVisible    = true;
    vec2  prevTexCoord = getPrevTexCoord(isVisible) + TAAJitter / viewSize;
    ivec2 prevTexelPos = getTexelPosFromTexCoord(ivec2(viewSize), prevTexCoord);

    vec4 history     = texelFetch(tex_moments_history, texelPos, 0);
    vec4 nowDirect   = texelFetch(tex_diffuse_direct, texelPos, 0);
    vec4 nowIndirect = texelFetch(tex_diffuse_indirect, texelPos, 0);
    vec4 oldDirect   = texture(tex_diffuse_direct_history, prevTexCoord);
    vec4 oldIndirect = texture(tex_diffuse_indirect_history, prevTexCoord);

    if ((abs(prevTexCoord - .5f).x <= .5f) && (abs(prevTexCoord - .5f).y <= .5f)) {
        if (isVisible) {
            history.b += 1.;
        } else {
            alpha     = 1.f;
            history.b = 1;
        }
    } else {
        alpha     = 1.f;
        history.b = 1;
    }
    if (any(isnan(oldDirect)) || any(isnan(oldIndirect))) {
        alpha     = 1.0;
        history.b = 1;
    }

    if (alpha > 0) {
        alpha = max(alpha, 1.f / history.b);
    }
    if (texCoord.x < .1 && texCoord.y < .1) { alpha = 1.f; }

    // Accumulate color and moment2 here.
    // integratedColor = mix(old.color, now.color, alpha);
    // integratedMoment2 = mix(old.moment, now.moment, alpha);

    float luminanceDirect   = luminance(nowDirect.rgb);
    float luminanceIndirect = luminance(nowIndirect.rgb);
    oldDirect.a             = history.r;
    oldIndirect.a           = history.g;
    nowDirect.a             = luminanceDirect * luminanceDirect;
    nowIndirect.a           = luminanceIndirect * luminanceIndirect;

    vec4 outDirect   = mix(oldDirect, nowDirect, alpha);
    vec4 outIndirect = mix(oldIndirect, nowIndirect, alpha);

    history.r = outDirect.a;
    history.g = outIndirect.a;

    // Calculate variance here.
    if ((history.b < 4) || (alpha > .99f)) {
        outDirect.a   = 1;
        outIndirect.a = 1;
    } else {
        float luminanceOutDirect   = luminance(outDirect.rgb);
        float luminanceOutIndirect = luminance(outIndirect.rgb);
        outDirect.a -= luminanceOutDirect * luminanceOutDirect;
        outIndirect.a -= luminanceOutIndirect * luminanceOutIndirect;
    }

    gl_FragData[0] = history;
    gl_FragData[1] = outDirect;
    gl_FragData[2] = outIndirect;
}