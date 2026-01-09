#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt -ob -f palette256 \
    -o ../../../bin/Vehicules/Textures/car \
    car.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_tex.bin ../../../bin/Vehicules/Textures/car_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_pal.bin ../../../bin/Vehicules/Textures/car_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_blue \
    -f palette256 \
    car_blue.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_blue_tex.bin ../../../bin/Vehicules/Textures/car_blue_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_blue_pal.bin ../../../bin/Vehicules/Textures/car_blue_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_gray \
    -f palette256 \
    car_gray.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_gray_tex.bin ../../../bin/Vehicules/Textures/car_gray_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_gray_pal.bin ../../../bin/Vehicules/Textures/car_gray_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_red \
    -f palette256 \
    car_red.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_red_tex.bin ../../../bin/Vehicules/Textures/car_red_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/car_red_pal.bin ../../../bin/Vehicules/Textures/car_red_pal.bin

