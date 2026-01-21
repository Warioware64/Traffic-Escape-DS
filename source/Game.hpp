#pragma once
#include <nds.h>
#include <filesystem.h>
#include <array>
#include <algorithm>
#include "PosVehicules.hpp"

namespace Game
{
    inline int textureID;
    inline float x_test;
    inline float y_test;
    inline float z_test;

    inline size_t idMesh;
    inline size_t idOrient;
    inline size_t idTex;
    
    inline void *car;
    inline void *grid;

    inline int edit_car;
    inline std::array<CarsStates, 5> cars;

    void Init();
    void Update();
}