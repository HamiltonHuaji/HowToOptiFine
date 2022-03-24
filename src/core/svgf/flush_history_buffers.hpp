#include "inc/buffers.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_worldpos;

#pragma rendertargets(2, 3)
void main() {
    gl_FragData[0] = texelFetch(tex_gbuffer, texelPos, 0);
    gl_FragData[1] = texelFetch(tex_worldpos, texelPos, 0);
}
