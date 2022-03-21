#!/bin/bash

echo "检查着色器代码..."
for fn in `ls shaders | grep -e ".*\.vsh"`;
do
    glslangValidator -S vert shaders/${fn}
done

for fn in `ls shaders | grep -e ".*\.fsh"`;
do
    glslangValidator -S frag shaders/${fn}
done

for fn in `ls shaders | grep -e ".*\.gsh"`;
do
    glslangValidator -S geom shaders/${fn}
done

for fn in `ls shaders | grep -e ".*\.csh"`;
do
    glslangValidator -S comp shaders/${fn}
done