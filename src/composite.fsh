
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/blockmapping.hpp"
#include "inc/buffers.hpp"
vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer;
uniform sampler2D tex_gbuffer_depth;
uniform sampler2D tex_worldpos;

#include "core/raytrace/diffuse.hpp"

#pragma rendertargets(6)
void main() {

    float depth = texelFetch(tex_gbuffer_depth, texelPos, 0).r;
    if (depth >= 1.0) {
        gl_FragData[0] = gl_FragData[1] = vec4(.36078, .63137, .84706, 1.);
        return;
    }

    GBufferData gd;
    gd.rawData = texelFetch(tex_gbuffer, texelPos, 0);
    unpackGBufferData(gd);
    if ((gd.emissivity > .0125) || isEmissive(gd.blockID)) {
        gl_FragData[0] = vec4(0);
        return;
    }
    vec3 binormal = normalize(cross(gd.tangent, gd.normal));
    mat3 tbn      = mat3(gd.tangent, binormal, gd.normal);

    vec3 worldPos = texture(tex_worldpos, texCoord).xyz;
    vec3 primaryRayEndVoxelPos = getVoxelPosFromLocalPos(worldPos - cameraPosition);

    vec4 illuminance = vec4(0);
    for (int i = 0; i < SAMPLE_PER_PIXEL; i++) {
        illuminance += raytrace_diffuse(primaryRayEndVoxelPos, tbn, uvec4(texelPos, frameCounter ^ i, 0));
    }
    gl_FragData[0] = illuminance / float(SAMPLE_PER_PIXEL);

    // gl_FragData[0].rgb = gd.diffuse * (1.f + (random3d(uvec3(texelPos, frameCounter)) - .5f));
    // gl_FragData[1].rgb = vec3(gd.smoothness, gd.metalness, gd.emissivity);
    
    // if (isCube(gd.blockID) && !isEmissive(gd.blockID)) { discard; }
    // if (isFence(gd.blockID) && (isWest(gd.blockID) || isNorth(gd.blockID))) { discard; }
    // if (isWallTorch(gd.blockID) && getFacing(gd.blockID)==facing_north) { discard; }
}
