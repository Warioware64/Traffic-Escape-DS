#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4 \
    -f palette256
    car4.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_tex.bin ../../../bin/Vehicules/Textures/car4_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_pal.bin ../../../bin/Vehicules/Textures/car4_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_grey \
    -f palette256 \
    car4_grey.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_grey_tex.bin ../../../bin/Vehicules/Textures/car4_grey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_grey_pal.bin ../../../bin/Vehicules/Textures/car4_grey_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightorange \
    -f palette256 \
    car4_lightorange.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightorange_tex.bin ../../../bin/Vehicules/Textures/car4_lightorange_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightorange_pal.bin ../../../bin/Vehicules/Textures/car4_lightorange_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightgrey \
    -f palette256 \
    car4_lightgrey.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightgrey_tex.bin ../../../bin/Vehicules/Textures/car4_lightgrey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightgrey_pal.bin ../../../bin/Vehicules/Textures/car4_lightgrey_pal.bin