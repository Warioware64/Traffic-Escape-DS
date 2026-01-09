#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car3 \
    -f palette256 \
    car3.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_tex.bin ../../../bin/Vehicules/Textures/car3_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_pal.bin ../../../bin/Vehicules/Textures/car3_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car3_yellow \
    -f palette256 \
    car3_yellow.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_yellow_tex.bin ../../../bin/Vehicules/Textures/car3_yellow_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_yellow_pal.bin ../../../bin/Vehicules/Textures/car3_yellow_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car3_red \
    -f palette256 \
    car3_red.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_red_tex.bin ../../../bin/Vehicules/Textures/car3_red_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car3_red_pal.bin ../../../bin/Vehicules/Textures/car3_red_pal.bin
