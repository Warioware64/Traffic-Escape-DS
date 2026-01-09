#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_shadow \
    -f a3i5 \
    car_shadow.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_shadow_alpha \
    -f a3i5 \
    car_shadow_alpha.png
    