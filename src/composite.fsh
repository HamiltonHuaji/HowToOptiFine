
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/blockmapping.hpp"
#include "inc/buffers.hpp"
vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_gbuffer_depth;

#include "inc/random.hpp"
vec3 random3d(uvec3 seed) {
    return vec3(pcg3d(seed)) * (1.f / float(0xffffffffu));
}

#pragma rendertargets(6, 7)
void main() {

    float depth = texelFetch(tex_gbuffer_depth, texelPos, 0).r;
    if (depth >= 1.0) {
        gl_FragData[0] = gl_FragData[1] = vec4(.36078, .63137, .84706, 1.);
        return;
    }

    GBufferData gd;
    gd.rawData = texelFetch(tex_gbuffer, texelPos, 0);
    unpackGBufferData(gd);
    if (gd.emissivity > .0125) {
        gl_FragData[0].rgb = gd.diffuse * gd.emissivity;
        gl_FragData[1] = vec4(0.f);
        return;
    }
        return;
    gl_FragData[0].rgb = gd.diffuse * (1.f + (random3d(uvec3(texelPos, frameCounter)) - .5f));
    gl_FragData[1].rgb = vec3(gd.smoothness, gd.metalness, gd.emissivity);
    
    // if (isCube(gd.blockID) && !isEmissive(gd.blockID)) { discard; }
    // if (isFence(gd.blockID) && (isWest(gd.blockID) || isNorth(gd.blockID))) { discard; }
    // if (isWallTorch(gd.blockID) && getFacing(gd.blockID)==facing_north) { discard; }
}
