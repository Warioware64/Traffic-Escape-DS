#pragma once
#include <array>
#include "PosVehicules.hpp"


constexpr LevelData level = {{
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 0, .orientation = 2, .tex = 0, .basepose = NULL, .grid2d = {0, 0}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 1, .orientation = 0, .tex = 4, .basepose = NULL, .grid2d = {3, 0}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 1, .orientation = 0, .tex = 5, .basepose = NULL, .grid2d = {0, 2}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 2, .orientation = 1, .tex = 9, .basepose = NULL, .grid2d = {1, 4}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 3, .orientation = 3, .tex = 10, .basepose = NULL, .grid2d = {0, 3}},
        {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 5, .orientation = 1, .tex = 18, .basepose = NULL, .grid2d = {0, 5}}
        //{.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = 1, .orientation = 0, .tex = 5, .basepose = NULL, .grid2d = {4, 3}},
    }};

constexpr LevelData cars2;
constexpr LevelData cars3;

constexpr std::array<LevelData, 3> Levels = {level};