#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8 \
    -f palette256 \
    Car8.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_tex.bin ../../../bin/Vehicules/Textures/Car8_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_pal.bin ../../../bin/Vehicules/Textures/Car8_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_grey \
    -f palette256 \
    Car8_grey.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_grey_tex.bin ../../../bin/Vehicules/Textures/Car8_grey_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_grey_pal.bin ../../../bin/Vehicules/Textures/Car8_grey_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_mail \
    -f palette256 \
    Car8_mail.png
    
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_mail_tex.bin ../../../bin/Vehicules/Textures/Car8_mail_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_mail_pal.bin ../../../bin/Vehicules/Textures/Car8_mail_pal.bin

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_purple \
    -f palette256 \
    Car8_purple.png

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_purple_tex.bin ../../../bin/Vehicules/Textures/Car8_purple_tex.bin
$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Textures/Car8_purple_pal.bin ../../../bin/Vehicules/Textures/Car8_purple_pal.bin