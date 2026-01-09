#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4 \
    -f palette256
    car4.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_grey \
    -f palette256 \
    car4_grey.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightorange \
    -f palette256 \
    car4_lightorange.png

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightgrey \
    -f palette256 \
    car4_lightgrey.png
    