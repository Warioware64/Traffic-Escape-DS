#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "Game.hpp"
#include "MainMenu.hpp"
#include "SaveData.hpp"

int main(int argc, char *argv[])
{
    // Initialize save system first (FAT must be initialized before nitroFS)
    SaveData::Init();

    // Show intro sequence (initializes nitroFS internally)
    MainMenu::Intro();

    // Show level selection and get selected level
    int selectedLevel = MainMenu::ShowLevelSelect();

    // Initialize game with selected level
    Game::Init(selectedLevel);

    // Main game loop
    while (1)
    {
        Game::Update();
    }

    return 0;
}
