#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5 \
    -f palette256 \
    car5.png

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5_green \
    -f palette256 \
    car5_green.png

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car5_grey \
    car5_grey.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car4_lightgrey \
    -f palette256 \
    car4_lightgrey.png