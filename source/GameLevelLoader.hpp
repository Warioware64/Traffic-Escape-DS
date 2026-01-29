#pragma once
#include <nds.h>
#include <array>
#include <string>
#include <algorithm>
#include "PosVehicules.hpp"

namespace GameLevelLoader
{
    inline std::array<std::string, 4> BG_name_list = {"sky1"};
    inline LevelData lev_data;
    void LoadBG(size_t bgID);
    void LoadLevel(int level);
    bool LoadLevelFromFile(const char* filename);  // Load from .bin file
    bool CollisionCheck(Grid2D grid2d, size_t excludeIndex);

}