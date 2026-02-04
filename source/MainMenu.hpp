#pragma once

#include <nds.h>
#include <filesystem.h>

namespace MainMenu
{
    // Menu states
    enum class State {
        INTRO,
        LEVEL_SELECT,
        GAME
    };

    // Current state
    inline State currentState = State::INTRO;

    // Selected level (0-based index)
    inline int selectedLevel = 0;

    // Total number of available levels
    constexpr int MAX_LEVELS = 20;
    inline int availableLevels = 11;  // Levels 1-11 exist in /Levels/

    // Initialize the menu system
    void Init();

    // Show intro sequence
    void Intro();

    // Show level selection screen and return selected level
    // Returns -1 if user wants to go back, or level number (0-based)
    int ShowLevelSelect();

    // Load and display top screen background
    void LoadTopScreenBG();

    // Load and display sub screen for level select
    void LoadSubScreenLevelSelect();

    // Handle touch input for level selection
    // Returns selected level or -1 if no selection
    int HandleLevelSelectTouch();

    // Draw level selection UI
    void DrawLevelSelectUI();

    // Legacy functions
    void TopBG();
    void SubBG();
}
