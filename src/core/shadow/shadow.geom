#include "inc/glsl.hpp"

layout(triangles) in;
layout(triangle_strip, max_vertices = 8) out;
// layout(points, max_vertices = 1) out;

flat in uint blockID[];     // 方块类型
flat in int  renderMode[];  // 是否写入 shadowmap 从而影响全局光照
flat in vec2 midTexCoord[]; // 占用的 texture 区域的中心点的 uv
flat in vec2 spriteSize[];  // 占用 texture 的尺寸
flat in vec3 color[];       // 顶点颜色(乘在 texture 纹理采样结果上)
flat in vec3 normal[];      // 法线(世界坐标)
flat in vec3 localPos[];    // 局部坐标, 即世界坐标减去摄像机位置

flat out vec4 data;

#include "inc/constants.hpp"
#include "inc/utils.hpp"
#include "inc/voxelspace.hpp"
#include "inc/uniforms.hpp"

void main() {
    // RSM
    for (int i=0; i<3; i++) {
        vec4 vertex = gl_in[i].gl_Position;
        vertex /= vertex.w;
        if (abs(vertex.x) >= 1) {
            return;
        }
        if (abs(vertex.y) >= 1) {
            return;
        }
        gl_Position = vec4(vertex.x * .5f + .5f, vertex.y * .5f + .5f, vertex.z, 1.f);
        data = vec4(KelvinToRGB(sunTemperature), 1.f);
        EmitVertex();
    }
    EndPrimitive();
    /*
     * 每个四边形包含两个三角形. 绘制其中的一个即可
     *
     * 2            2 - 1
     * |                |
     * 0 - 1            0
     *
     * 会被绘制    不会被绘制
     */
    if (abs(dot(localPos[0] - localPos[1], localPos[2] - localPos[1])) < 0.001) { return; }
    if (length(localPos[0]) > voxelR) { return; }

    if (renderMode[0] == 0) {
        // 三角形中心的坐标. 沿法线向内偏移
        // 此时 centerWorldPos 一定落在对应方块的体积内部
        vec3 centerLocalPos = (localPos[0] + localPos[1] + localPos[2]) / 3.f - normal[0] / 1024.f;

        ivec3 centerBlockPos = getBlockPosFromLocalPos(centerLocalPos);

        VoxelData vd;
        vd.midTexCoord = midTexCoord[0];
        vd.spriteSize  = spriteSize[0];
        vd.color       = color[0];
        vd.blockID     = blockID[0];

        packVoxelData(vd);

        ivec2 texelPos = getTexelPosFromVoxelPos(centerBlockPos);
        data           = vd.rawData;

        gl_Position    = shadow_getGLPositionFromTexelPos(texelPos, .5f);
        EmitVertex();
        gl_Position    = shadow_getGLPositionFromTexelPos(texelPos + ivec2(1, 0), .5f);
        EmitVertex();
        gl_Position    = shadow_getGLPositionFromTexelPos(texelPos + ivec2(0, 1), .5f);
        EmitVertex();
        gl_Position    = shadow_getGLPositionFromTexelPos(texelPos + ivec2(1, 1), .5f);
        EmitVertex();
        EndPrimitive();
    }
}