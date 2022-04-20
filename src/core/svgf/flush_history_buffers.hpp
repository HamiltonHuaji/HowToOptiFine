#include "inc/buffers.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_localpos;

out vec4 outColor0;
out vec4 outColor1;

#pragma rendertargets(2, 3)
void main() {
    outColor0 = texelFetch(tex_gbuffer, texelPos, 0);
    outColor1 = texelFetch(tex_localpos, texelPos, 0);
}
