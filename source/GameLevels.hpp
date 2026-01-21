#pragma once
#include <array>
#include "PosVehicules.hpp"


constexpr LevelData level = {{
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 0, .orientation = 1, .tex = 0, .basepose = NULL, .grid2d = {0, 0}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 0, .orientation = 0, .tex = 0, .basepose = NULL, .grid2d = {0, 0}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 0, .orientation = 0, .tex = 0, .basepose = NULL, .grid2d = {0, 0}},
    }};

constexpr LevelData cars2;
constexpr LevelData cars3;

constexpr std::array<LevelData, 3> Levels = {level};