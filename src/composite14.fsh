#include "inc/glsl.hpp"
#include "inc/buffers.hpp"
#include "inc/uniforms.hpp"
#include "inc/gbuffer.hpp"
#include "inc/blockmapping.hpp"

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_diffuse_direct;

vec2 texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

#pragma rendertargets(6)
void main() {
    GBufferData gd;
    gd.rawData = texelFetch(tex_gbuffer, texelPos, 0);
    unpackGBufferData(gd);
    vec3 emissiveColor = vec3(0);
    if ((gd.emissivity > .0125) || isEmissive(gd.blockID)) {
        gl_FragData[0] = vec4(gd.diffuse, 1.0f);
    } else {
        vec3 illuminance = texture(colortex6, texCoord).rgb;
        gl_FragData[0] = vec4(gd.diffuse * illuminance, 1.0f);
    }
}
