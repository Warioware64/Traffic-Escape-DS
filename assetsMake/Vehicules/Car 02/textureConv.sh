#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2 \
    -f palette256 \
    car2.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_tex.bin ../../../bin/Vehicules/Textures/car2_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_pal.bin ../../../bin/Vehicules/Textures/car2_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2_black \
    -f palette256 \
    car2_black.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_black_tex.bin ../../../bin/Vehicules/Textures/car2_black_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_black_pal.bin ../../../bin/Vehicules/Textures/car2_black_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2_red \
    -f palette256 \
    car2_red.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_red_tex.bin ../../../bin/Vehicules/Textures/car2_red_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car2_red_pal.bin ../../../bin/Vehicules/Textures/car2_red_pal.bin