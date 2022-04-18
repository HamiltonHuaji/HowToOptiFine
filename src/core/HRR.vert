#include "inc/glsl.hpp"
#include "inc/constants.hpp"
void main() {
#if (HRR==1)
    vec4 transformed = ftransform();
    transformed /= transformed.w;
    transformed.xy = transformed.xy * .5 -.5;
    gl_Position = transformed;
#else
    gl_Position = ftransform();
#endif
}
