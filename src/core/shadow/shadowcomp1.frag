#include "inc/buffers.hpp"
#include "inc/constants.hpp"
#include "inc/glsl.hpp"
#include "inc/uniforms.hpp"
vec2  texCoord = gl_FragCoord.xy / float(shadowMapResolution);
ivec2 texelPos = ivec2(gl_FragCoord.xy);

uniform sampler2D shadowcolor0;
uniform sampler2D shadowcolor1;

out vec4 outColor0;
out vec4 outColor1;

const float offset[3] = float[3](0.0, 1.3846153846, 3.2307692308);
const float weight[3] = float[3](0.2270270270, 0.3162162162, 0.0702702703);

const float texelSize = 1.f / shadowMapResolution;

#pragma rendertargets(0, 1)
void main() {
    if ((texelPos.x * 2 <= shadowMapResolution) || (texelPos.y * 2 <= shadowMapResolution)) {
        outColor0 = texelFetch(shadowcolor0, texelPos, 0);
        outColor1 = texelFetch(shadowcolor1, texelPos, 0);
    } else {
        outColor0 = texture(shadowcolor0, texCoord) * weight[0];
        for (int i = 1; i < 3; i++) {
            outColor0 += texture(shadowcolor0, texCoord + vec2(0.0f, offset[i]) * texelSize) * weight[i];
            outColor0 += texture(shadowcolor0, texCoord - vec2(0.0f, offset[i]) * texelSize) * weight[i];
        }
        outColor1 = texture(shadowcolor1, texCoord) * weight[0];
        for (int i = 1; i < 3; i++) {
            outColor1 += texture(shadowcolor1, texCoord + vec2(0.0f, offset[i]) * texelSize) * weight[i];
            outColor1 += texture(shadowcolor1, texCoord - vec2(0.0f, offset[i]) * texelSize) * weight[i];
        }
    }
}
