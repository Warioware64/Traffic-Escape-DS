#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$OBJ2DL \
    --input gridv2_final_fixed2.obj \
    --output ../../bin/Vehicules/Meshes/gridv2_final_fixed2.bin \
    --scale 0.5 \
    --texture 8 16

#$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Meshes/gridv2.bin ../../../bin/Vehicules/Meshes/gridv2.bin
