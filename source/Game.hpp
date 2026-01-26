#pragma once
#include <nds.h>
#include <iostream>
#include <filesystem.h>
#include <array>
#include <vector>
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
    inline LevelData cars;

    // Touch screen state
    inline int touch_selected_car = -1;
    inline int touch_start_x = 0;
    inline int touch_start_y = 0;
    inline int touch_last_x = 0;
    inline int touch_last_y = 0;
    inline bool touch_dragging = false;

    // Victory state
    inline bool level_won = false;

    // Touch helper functions
    Grid2D ScreenToGrid(int px, int py);
    int FindCarAtGrid(Grid2D grid);
    void HandleTouch();
    void DrawDebugTouchZone();
    bool CheckVictory();

    void Init();
    void Update();
}