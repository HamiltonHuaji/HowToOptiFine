#include "inc/constants.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
#include "inc/utils.hpp"

uniform sampler2D tex;

flat in vec4 data;
flat in int  isShadowMap;
in vec2      ftexCoord;

out vec4 outColor0;
out vec4 outColor1;

#pragma rendertargets(0, 1)

void main() {
    if (isShadowMap > 0) {
        vec4 diffuse = texture(tex, ftexCoord);
        if (diffuse.a < .0125) {
            // discard;
        } // 丢弃树叶等材质的镂空部分
        outColor0 = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0, 1);
    } else {
        outColor0 = data;
    }
}
