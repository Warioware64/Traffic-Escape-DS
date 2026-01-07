#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car4 \
    car4.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car4_grey \
    car4_grey.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car4_lightorange \
    car4_lightorange.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car4_lightgrey \
    car4_lightgrey.png \
    -f tex4x4