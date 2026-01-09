#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car6 \
    -f palette256 \
    car6.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car6_tex.bin ../../../bin/Vehicules/Textures/car6_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car6_pal.bin ../../../bin/Vehicules/Textures/car6_pal.bin