
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/blockmapping.hpp"
#include "inc/buffers.hpp"
vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;

#include "inc/random.hpp"
vec3 random3d(uvec3 seed) {
    return vec3(pcg3d(seed)) * (1.f / float(0xffffffffu));
}

#pragma rendertargets(6, 7)
void main() {
    GBufferData gd;
    gd.rawData = texelFetch(tex_gbuffer, texelPos, 0);
    unpackGBufferData(gd);
    gl_FragData[0].rgb = gd.diffuse * (1.f + (random3d(uvec3(texelPos, frameCounter)) - .5f));
    gl_FragData[1].rg  = vec2(gd.smoothness, gd.metalness);
    
    // if (isCube(gd.blockID) && !isEmissive(gd.blockID)) { discard; }
    // if (isFence(gd.blockID) && (isWest(gd.blockID) || isNorth(gd.blockID))) { discard; }
    // if (isWallTorch(gd.blockID) && getFacing(gd.blockID)==facing_north) { discard; }
}
