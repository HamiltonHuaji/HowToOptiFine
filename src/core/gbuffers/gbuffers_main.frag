#pragma once
#include "inc/glsl.hpp"

uniform sampler2D tex;
uniform sampler2D specular;

flat in uint blockID;  // 方块种类
in vec2      texCoord; // 纹理坐标
in vec3      color;    // 顶点色
in vec3      normal;   // 面法线(世界坐标)
in vec3      tangent;  // 面切线(世界坐标)
in vec3      localPos; // 世界坐标减摄像机位置
in vec3      velocity; // 运动向量

#include "inc/blockmapping.hpp"
#include "inc/gbuffer.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"
out vec4 outColor0;
out vec4 outColor1;
out vec4 outColor2;
out vec4 outColor3;

#pragma rendertargets(0, 1, 4, 15)
void main() {
    GBufferData gd;

    vec4 diffuse = sRGB2linear(texture(tex, texCoord) * vec4(color, 1.f));
    if (diffuse.a < .0125) { discard; } // 丢弃树叶等材质的镂空部分

    vec4 spec  = texture(specular, texCoord).rgba;
    gd.blockID = blockID;
    gd.diffuse = diffuse.rgb;
    if (gl_FrontFacing) {
        gd.normal = normal;
    } else {
        gd.normal = -normal;
    }
    gd.smoothness = spec.r;
    gd.metalness  = spec.g;
    gd.emissivity = spec.a; // LabPBR
    gd.tangent    = tangent;
    packGBufferData(gd);
    outColor0 = gd.rawData;
    outColor1 = vec4(localPos, 1.f);

    vec3 currProjPos = gl_FragCoord.xyz / vec3(viewSize, 1.f);
#ifndef GBUFFERS_ENTITIES
    vec4 projPosPrev = gbufferPreviousProjection * gbufferPreviousModelView * vec4(localPos + cameraPosition - previousCameraPosition, 1.f);
#else
    vec4 viewPosPrev = gbufferModelView * vec4(localPos, 1.0);
    viewPosPrev.xyz -= velocity.xyz;
    vec4 projPosPrev = gbufferPreviousProjection * vec4(viewPosPrev.xyz, 1.0);
#endif
    projPosPrev /= projPosPrev.w;
    projPosPrev += 1.f;
    projPosPrev /= 2.f;

    outColor2 = vec4(projPosPrev.xyz - currProjPos, 0.f);

    outColor3 = outColor2;
}