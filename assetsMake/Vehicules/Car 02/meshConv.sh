#!/bin/bash

OBJ2DL=/opt/wonderful/thirdparty/blocksds/external/nitro-engine/tools/obj2dl/obj2dl.py

$OBJ2DL \
    --input Car2.obj \
    --output ../../../nitrofiles/Vehicules/Meshes/Car2.bin \
    --scale 0.5 \
    --texture 128 128