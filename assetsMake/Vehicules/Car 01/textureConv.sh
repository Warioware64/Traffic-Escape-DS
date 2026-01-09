#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt -ob -f palette256 \
    -o ../../../bin/Vehicules/Textures/car \
    car.png


$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_blue \
    -f palette256 \
    car_blue.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_gray \
    -f palette256 \
    car_gray.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/car_red \
    -f palette256 \
    car_red.png
    

