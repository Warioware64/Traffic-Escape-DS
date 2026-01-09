#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2 \
    -f palette256 \
    car2.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2_black \
    -f palette256 \
    car2_black.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car2_red \
    -f palette256 \
    car2_red.png
    