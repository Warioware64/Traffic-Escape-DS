#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "Game.hpp"
#include "MainMenu.hpp"

int main(int argc, char *argv[])
{
    // Show intro sequence
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
