#include "inc/glsl.hpp"

attribute vec4 at_tangent;
attribute vec3 mc_Entity;
attribute vec2 mc_midTexCoord;
#include "inc/blockmapping.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

flat out uint blockID;     // 方块类型
flat out int  renderMode;  // 是否写入 shadowmap 从而影响全局光照
flat out vec2 midTexCoord; // 占用的 texture 区域的中心点的 uv
flat out vec2 spriteSize;  // 占用 texture 的尺寸
flat out vec3 color;       // 顶点颜色(乘在 texture 纹理采样结果上), 对于同一个方块是一样的(用于表示不同群系下的草方块等)
flat out vec3 normal;      // 法线(世界坐标)
flat out vec3 localPos;    // 局部坐标, 即世界坐标减去摄像机位置

void main() {
    blockID    = getBlockID(mc_Entity);
    if (isCube(blockID) || isFacingCube(blockID)) {
        renderMode = 0;
    } else {
        renderMode = 1;
    }

    midTexCoord = mc_midTexCoord;

    // 纹理坐标
    vec2 texCoord = gl_MultiTexCoord0.st;
    spriteSize    = abs(texCoord - mc_midTexCoord) * 2.f;

    color    = gl_Color.rgb;
    normal   = normalize(mat3(shadowModelViewInverse) * gl_NormalMatrix * gl_Normal);
    localPos = (shadowModelViewInverse * gl_ModelViewMatrix * gl_Vertex).xyz;

    // 无实义
    gl_Position = ftransform();
}
