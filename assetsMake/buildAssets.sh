#!/bin/bash

declare -a vehicules_scripts=("Car 01" "Car 02" "Car 03" "Car 04" "Car 05" "Car 06" "Car 07" "Car 08")

for i in "${vehicules_scripts[@]}"
do
    cd Vehicules/"$i"
    ./meshConv.sh
    ./textureConv.sh
    cd ../../
done