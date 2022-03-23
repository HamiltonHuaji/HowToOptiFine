#include "inc/glsl.hpp"
flat in vec4 data;

void main() {
    gl_FragData[0] = data;
}
