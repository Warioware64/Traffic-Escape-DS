#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5 \
    -f palette256 \
    car5.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_tex.bin ../../../bin/Vehicules/Textures/car5_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_pal.bin ../../../bin/Vehicules/Textures/car5_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5_green \
    -f palette256 \
    car5_green.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_green_tex.bin ../../../bin/Vehicules/Textures/car5_green_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_green_pal.bin ../../../bin/Vehicules/Textures/car5_green_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5_grey \
    car5_grey.png \
    -f tex4x4

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_grey_tex.bin ../../../bin/Vehicules/Textures/car5_grey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car5_grey_pal.bin ../../../bin/Vehicules/Textures/car5_grey_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightgrey \
    -f palette256 \
    car4_lightgrey.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightgrey_tex.bin ../../../bin/Vehicules/Textures/car4_lightgrey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car4_lightgrey_pal.bin ../../../bin/Vehicules/Textures/car4_lightgrey_pal.bin