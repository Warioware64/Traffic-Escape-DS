#pragma once
#include <array>
#include <algorithm>
#include "PosVehicules.hpp"

namespace GameLevelLoader
{
    inline LevelData lev_data;
    void LoadLevel(int level);
    bool LoadLevelFromFile(const char* filename);  // Load from .bin file
    bool CollisionCheck(Grid2D grid2d, size_t excludeIndex);

}