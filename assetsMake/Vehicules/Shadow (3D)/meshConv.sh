#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

$OBJ2DL \
    --input Car_Shadow.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car_Shadow.bin \
    --scale 0.5 \
    --texture 64 128