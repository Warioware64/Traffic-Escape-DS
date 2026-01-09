#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8 \
    -f palette256 \
    car8.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_grey \
    -f palette256 \
    Car8_grey.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_mail \
    -f palette256 \
    Car8_mail.png
    

$PTEXCONV \
    -gt \
    -ob \
    -o ../../../bin/Vehicules/Textures/Car8_purple \
    -f palette256 \
    Car8_purple.png
    