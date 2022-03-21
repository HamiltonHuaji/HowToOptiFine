#include "inc/buffers.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_worldpos;

uniform sampler2D tex_diffuse_direct;
uniform sampler2D tex_diffuse_indirect;

#pragma rendertargets(2, 3, 8, 9)
void main() {
    gl_FragData[0] = texelFetch(tex_gbuffer, texelPos, 0);
    gl_FragData[1] = texelFetch(tex_worldpos, texelPos, 0);
    gl_FragData[2] = texelFetch(tex_diffuse_direct, texelPos, 0);
    gl_FragData[3] = texelFetch(tex_diffuse_indirect, texelPos, 0);
}
