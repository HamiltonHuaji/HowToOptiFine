'''
生成 shaders/final.fsh, 以显示多张纹理和调试变量
'''

from typing import List
import pystache, os

template_patch = '''
#version 430 compatibility

#define GAMMA 1.8 // [0.5 1.0 1.5 1.6 1.7 1.8 1.9 2.0 2.1 2.2 2.3 2.4 2.5 3.0 3.5 4.0]
#define GAMMA_CORRECTION 0

{{ #sources }}
uniform sampler2D {{ source }};
{{ /sources }}

uniform float viewWidth;
uniform float viewHeight;
vec2 viewSize = vec2(viewWidth, viewHeight);
vec2 texCoord = gl_FragCoord.xy / viewSize;
ivec2 texelPos = ivec2(gl_FragCoord.xy);

void main() {
#if (GAMMA_CORRECTION==0)
    vec4 finalColor = texture({{ background }}, texCoord);
    vec2 relative;
    {{ #patch }}
    relative = (texCoord - vec2({{xbase}}, {{ybase}})) / vec2({{xscale}}, {{yscale}});
    if(relative.x >= 0 && relative.x < 1 && relative.y >= 0 && relative.y < 1) {
        finalColor = texture({{source}}, relative);
    }
    {{ /patch }}
    gl_FragData[0] = vec4(pow(finalColor.rgb, vec3(1.f / GAMMA)), 1);
#else
    if (texCoord.x < .5) {
        if (((texelPos.x ^ texelPos.y) & 1) > 0) {
            gl_FragData[0] = vec4(pow(vec3(0,0,0), vec3(1.f / GAMMA)), 1.f);
        } else {
            gl_FragData[0] = vec4(pow(vec3(1,1,1), vec3(1.f / GAMMA)), 1.f);
        }
    } else {
        gl_FragData[0] = vec4(pow(vec3(.5, .5, .5), vec3(1.f / GAMMA)), 1.f);
    }
#endif
}
'''


class Patch:
    def __init__(self, source, xbase=.5, ybase=.5, xscale=.5, yscale=.5, reverse=False) -> None:
        self.source = source
        if not reverse:
            self.xbase = xbase
            self.ybase = ybase
            self.xscale = xscale
            self.yscale = yscale
        else:
            self.xbase = xscale + xbase
            self.ybase = yscale + ybase
            self.xscale = -xscale
            self.yscale = -yscale


class Screen:
    def __init__(self, background="colortex6") -> None:
        self.patches = []
        self.sources = set()
        self.sources.add(background)
        self.background = background

    def add(self, patch: Patch):
        self.patches.append({
            "source": patch.source,
            "xbase": patch.xbase,
            "ybase": patch.ybase,
            "xscale": patch.xscale,
            "yscale": patch.yscale,
        })
        self.sources.add(patch.source)

    def add_targets(self, targets: List[str], maxwidth=None):
        width = .72 / len(targets)
        height = .64 / .36 * width
        if maxwidth:
            height *= maxwidth / width
            width = maxwidth
        for i, t in enumerate(targets):
            self.add(Patch(t, xbase=i * width, ybase=1 - height, xscale=width, yscale=height))

    def build(self):
        return pystache.render(template_patch, {
            "sources": [{"source": source} for source in self.sources],
            "patch": self.patches,
            "background": self.background
        })

screen = Screen(background="colortex6")

# screen.add_targets(["colortex6", "colortex7"], maxwidth=.25)
# screen.add_targets(["shadowcolor0", "colortex15"], maxwidth=.25)
os.makedirs(os.path.dirname("shaders/final.fsh"), exist_ok=True)
with open("shaders/final.fsh", "w") as f:
    f.write(screen.build())
print("已更新 shaders/final.fsh")