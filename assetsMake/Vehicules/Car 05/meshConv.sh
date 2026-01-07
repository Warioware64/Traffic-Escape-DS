#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

$OBJ2DL \
    --input Car5.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car5.bin \
    --scale 0.5 \
    --texture 128 128

$OBJ2DL \
    --input Car5_Police.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car5_Police.bin \
    --scale 0.5 \
    --texture 128 128

$OBJ2DL \
    --input Car5_Taxi.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car5_Taxi.bin \
    --scale 0.5 \
    --texture 128 128