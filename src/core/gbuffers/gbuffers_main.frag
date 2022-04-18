#pragma once
#include "inc/glsl.hpp"

uniform sampler2D tex;
uniform sampler2D specular;

flat in uint blockID;      // 方块种类
in vec2      texCoord;     // 纹理坐标
in vec3      color;        // 顶点色
in vec3      normal;       // 面法线(世界坐标)
in vec3      tangent;      // 面切线(世界坐标)
in vec3      localPos;     // 世界坐标减摄像机位置
in vec3      motionVector; // 运动向量

#include "inc/blockmapping.hpp"
#include "inc/gbuffer.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

#pragma rendertargets(0, 1, 4, 15)
void main() {
    GBufferData gd;

    vec4 diffuse = sRGB2linear(texture(tex, texCoord) * vec4(color, 1.f));
    if (diffuse.a < .0125) { discard; } // 丢弃树叶等材质的镂空部分

    vec4 spec     = texture(specular, texCoord).rgba;
    gd.blockID    = blockID;
    gd.diffuse    = diffuse.rgb;
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
    gl_FragData[0] = gd.rawData;
    gl_FragData[1] = vec4(localPos, 1.f);
#ifndef GBUFFERS_ENTITIES
    vec4 projPosPrev = gbufferPreviousProjection * gbufferPreviousModelView * vec4(localPos + cameraPosition - previousCameraPosition, 1.f);
    projPosPrev /= projPosPrev.w;
    projPosPrev += 1.f;
    projPosPrev /= 2.f;

    vec3 curr = gl_FragCoord.xyz / vec3(viewWidth, viewHeight, 1.f);
    gl_FragData[2] = vec4(projPosPrev.xyz - curr, 0.f);
#else
    gl_FragData[2] = vec4(motionVector * .5f, 0.f);
#endif
    // gl_FragData[3] = vec4(motionVector, 0);
    gl_FragData[3] = gl_FragData[2];
}