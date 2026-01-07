#!/bin/bash

PTEXCONV=/opt/wonderful/thirdparty/blocksds/external/ptexconv/ptexconv

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/Car8 \
    car8.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/Car8_grey \
    Car8_grey.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/Car8_mail \
    Car8_mail.png \
    -f tex4x4

$PTEXCONV \
    -gt \
    -og \
    -o ../../../nitrofiles/Vehicules/Textures/Car8_purple \
    Car8_purple.png \
    -f tex4x4