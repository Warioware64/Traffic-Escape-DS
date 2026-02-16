#pragma once
//#include <dsf.h>
#include <nds.h>
#include <iostream>
#include <filesystem.h>
#include <array>
#include <vector>
#include <algorithm>
#include "PosVehicules.hpp"

namespace Game
{
    // Game state enum
    enum class GameState {
        PLAYING,
        PAUSED,
        VICTORY
    };

    // Update result - tells main loop what to do
    enum class UpdateResult {
        CONTINUE,       // Keep playing
        QUIT_TO_MENU,   // Return to level selection
        RETRY_LEVEL,    // Restart current level
        NEXT_LEVEL      // Load next level (victory)
    };

    // Menu option for pause/victory screens
    enum class MenuOption {
        NONE = -1,
        CONTINUE = 0,   // Only in pause menu
        RETRY = 1,
        QUIT = 2,
        NEXT_LEVEL = 3  // Only in victory menu
    };

    inline int textureID;
    inline int frame;
    inline int renderPass = 0;  // For dual 3D: 0 = first pass, 1 = second pass

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

    // Game state
    inline GameState gameState = GameState::PLAYING;
    inline MenuOption selectedOption = MenuOption::NONE;
    inline bool isNewRecord = false;

    // Timer state (counts frames, 60fps)
    inline uint32_t timer_frames = 0;
    inline bool timer_running = true;

    // Touch helper functions
    Grid2D ScreenToGrid(int px, int py);
    int FindCarAtGrid(Grid2D grid);
    void HandleTouch();
    void DrawDebugTouchZone();
    bool CheckVictory();

    // Menu functions
    void DrawPauseMenu();
    void DrawVictoryMenu();
    MenuOption HandleMenuTouch(bool isPauseMenu);

    // Current level index
    inline int currentLevel = 0;

    // Total available levels
    inline int totalLevels = 11;

    // Custom level flag
    inline bool isCustomLevel = false;
    inline char customLevelPath[256] = {0};

    void Init(int level = 0);
    void InitCustom(const char* jsonPath);  // Init from custom JSON level
    UpdateResult Update();
    void Cleanup();  // Clean up textures and meshes when leaving level
}