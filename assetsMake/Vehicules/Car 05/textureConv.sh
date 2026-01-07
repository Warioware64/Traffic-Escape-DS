#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car5 \
    car5.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car5_green \
    car5_green.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car5_grey \
    car5_grey.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car4_lightgrey \
    car4_lightgrey.png \
    -f tex4x4