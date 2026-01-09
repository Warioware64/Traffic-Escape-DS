#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7 \
    -f palette256 \
    car7.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_tex.bin ../../../bin/Vehicules/Textures/car7_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_pal.bin ../../../bin/Vehicules/Textures/car7_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7_black \
    -f palette256 \
    car7_black.png \
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_black_tex.bin ../../../bin/Vehicules/Textures/car7_black_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_black_pal.bin ../../../bin/Vehicules/Textures/car7_black_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7_brown \
    -f palette256 \
    car7_brown.png \
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_brown_tex.bin ../../../bin/Vehicules/Textures/car7_brown_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_brown_pal.bin ../../../bin/Vehicules/Textures/car7_brown_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7_green \
    -f palette256 \
    car7_green.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_green_tex.bin ../../../bin/Vehicules/Textures/car7_green_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_green_pal.bin ../../../bin/Vehicules/Textures/car7_green_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7_grey \
    -f palette256 \
    car7_grey.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_grey_tex.bin ../../../bin/Vehicules/Textures/car7_grey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_grey_pal.bin ../../../bin/Vehicules/Textures/car7_grey_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car7_red \
    -f palette256 \
    car7_red.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_red_tex.bin ../../../bin/Vehicules/Textures/car7_red_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car7_red_pal.bin ../../../bin/Vehicules/Textures/car7_red_pal.bin