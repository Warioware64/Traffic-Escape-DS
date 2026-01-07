#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car_shadow \
    car_shadow.png \
    -f a3i5

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/car_shadow_alpha \
    car_shadow_alpha.png \
    -f a3i5