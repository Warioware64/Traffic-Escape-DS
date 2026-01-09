#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

$OBJ2DL \
    --input Car3.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car3.bin \
    --scale 0.5 \
    --texture 128 128