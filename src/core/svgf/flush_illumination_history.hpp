#include "inc/buffers.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"

vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_diffuse_direct;
out vec4 outColor0;

#pragma rendertargets(7)
void main() {
    vec4 color = vec4(texelFetch(tex_diffuse_direct, texelPos, 0).rgb, 1.0f);
    if (any(isnan(color))) {
        color = vec4(0);
    }
    outColor0 = color;
}
