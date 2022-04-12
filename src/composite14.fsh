
#include "inc/gbuffer.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/blockmapping.hpp"
#include "inc/buffers.hpp"
#include "inc/constants.hpp"
vec2  texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D tex_gbuffer_depth;
uniform sampler2D tex_gbuffer;
uniform sampler2D tex_diffuse_direct;

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.f, 1.f);
}

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
        gl_FragData[0].rgb = gd.diffuse;
    } else {
#if     (COMPOSITE_OPTION==0)
        gl_FragData[0].rgb = gd.diffuse;
#elif   (COMPOSITE_OPTION==1)
        gl_FragData[0].rgb = texelFetch(tex_diffuse_direct, texelPos, 0).rgb;
#elif   (COMPOSITE_OPTION==2)
        gl_FragData[0].rgb = gd.diffuse * texelFetch(tex_diffuse_direct, texelPos, 0).rgb;
#elif   (COMPOSITE_OPTION==3)
        gl_FragData[0].rgb = aces(gd.diffuse * texelFetch(tex_diffuse_direct, texelPos, 0).rgb);
#endif
    }

    // gl_FragData[1].rgb = vec3(gd.smoothness, gd.metalness, gd.emissivity);
    
    // if (isCube(gd.blockID) && !isEmissive(gd.blockID)) { discard; }
    // if (isFence(gd.blockID) && (isWest(gd.blockID) || isNorth(gd.blockID))) { discard; }
    // if (isWallTorch(gd.blockID) && getFacing(gd.blockID)==facing_north) { discard; }
}
