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

    while (true)
    {
        // Show level selection and get selected level
        int selectedLevel = MainMenu::ShowLevelSelect();

        // Game loop for current level
        bool inGame = true;
        while (inGame)
        {
            // Initialize game with selected level
            Game::Init(selectedLevel);

            // Play until user quits, retries, or goes to next level
            bool playingLevel = true;
            while (playingLevel)
            {
                Game::UpdateResult result = Game::Update();

                switch (result)
                {
                    case Game::UpdateResult::CONTINUE:
                        // Keep playing
                        break;

                    case Game::UpdateResult::QUIT_TO_MENU:
                        // Return to level selection
                        playingLevel = false;
                        inGame = false;
                        break;

                    case Game::UpdateResult::RETRY_LEVEL:
                        // Restart current level
                        playingLevel = false;
                        // inGame stays true, so we'll re-init the same level
                        break;

                    case Game::UpdateResult::NEXT_LEVEL:
                        // Go to next level if available
                        if (selectedLevel < Game::totalLevels - 1)
                        {
                            selectedLevel++;
                            playingLevel = false;
                            // inGame stays true, so we'll init the next level
                        }
                        else
                        {
                            // No more levels, return to menu
                            playingLevel = false;
                            inGame = false;
                        }
                        break;
                }
            }
        }
    }

    return 0;
}
