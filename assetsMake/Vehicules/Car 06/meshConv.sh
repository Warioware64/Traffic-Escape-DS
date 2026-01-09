#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$OBJ2DL \
    --input Car6.obj \
    --output ../../../bin/Vehicules/Meshes/Car6.bin \
    --scale 0.5 \
    --texture 128 128

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Meshes/Car6.bin ../../../bin/Vehicules/Meshes/Car6.bin