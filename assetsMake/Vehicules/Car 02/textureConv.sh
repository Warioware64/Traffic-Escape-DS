#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car2 \
    car2.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car2_black \
    car2_black.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car2_red \
    car2_red.png \
    -f tex4x4