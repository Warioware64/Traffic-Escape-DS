#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

WF_NNPACK_LZSS=/opt/wonderful/bin/wf-nnpack-lzss

$OBJ2DL \
    --input Car5.obj \
    --output ../../../bin/Vehicules/Meshes/Car5.bin \
    --scale 0.5 \
    --texture 128 128

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Meshes/Car5.bin ../../../bin/Vehicules/Meshes/Car5.bin

$OBJ2DL \
    --input Car5_Police.obj \
    --output ../../../bin/Vehicules/Meshes/Car5_Police.bin \
    --scale 0.5 \
    --texture 128 128

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Meshes/Car5_Police.bin ../../../bin/Vehicules/Meshes/Car5_Police.bin

$OBJ2DL \
    --input Car5_Taxi.obj \
    --output ../../../bin/Vehicules/Meshes/Car5_Taxi.bin \
    --scale 0.5 \
    --texture 128 128

$WF_NNPACK_LZSS -ewf ../../../bin/Vehicules/Meshes/Car5_Taxi.bin ../../../bin/Vehicules/Meshes/Car5_Taxi.bin