#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "Game.hpp"
#include "MainMenu.hpp"
#include "SaveData.hpp"
#include "MusicStream.hpp"
#include "CustomLevelBrowser.hpp"
#include "OnlineLevelBrowser.hpp"
#include "BGFont.hpp"

// Play a custom level game loop (retry supported, quit returns to browser)
static void PlayCustomLevel(const char* jsonPath)
{
    bool inGame = true;
    while (inGame)
    {
        Game::InitCustom(jsonPath);

        bool playingLevel = true;
        while (playingLevel)
        {
            Game::UpdateResult result = Game::Update();
            MusicStream::Update();

            switch (result)
            {
                case Game::UpdateResult::CONTINUE:
                    break;
                case Game::UpdateResult::QUIT_TO_MENU:
                    playingLevel = false;
                    inGame = false;
                    break;
                case Game::UpdateResult::RETRY_LEVEL:
                    playingLevel = false;
                    // inGame stays true -> re-init same custom level
                    break;
                case Game::UpdateResult::NEXT_LEVEL:
                    // No next level for custom, return to browser
                    playingLevel = false;
                    inGame = false;
                    break;
            }
        }
    }
}


int main(int argc, char *argv[])
{
    // Initialize save system first (FAT must be initialized before nitroFS)
    SaveData::Init();

    // Show intro sequence (initializes nitroFS internally)
    MainMenu::Intro();

    // Start background music (must be after nitroFSInit in Intro)
    MusicStream::Init("nitro:/audio/blossom.wav");

    while (true)
    {
        // Show level selection and get selected level
        int selectedLevel = MainMenu::ShowLevelSelect();

        // Handle custom levels
        if (selectedLevel == MainMenu::SELECT_CUSTOM_LEVELS)
        {
            // Custom level browser loop
            bool browsing = true;
            while (browsing)
            {
                CustomLevelBrowser::Result result = CustomLevelBrowser::Show();
                if (result == CustomLevelBrowser::Result::PLAY)
                {
                    PlayCustomLevel(CustomLevelBrowser::GetSelectedPath());
                    // After playing, return to browser
                }
                else
                {
                    browsing = false;
                }
            }
            continue;  // Return to main level select
        }

        // Handle online levels
        if (selectedLevel == MainMenu::SELECT_ONLINE_LEVELS)
        {
            bool browsing = true;
            while (browsing)
            {
                OnlineLevelBrowser::Result result = OnlineLevelBrowser::Show();
                if (result == OnlineLevelBrowser::Result::PLAY)
                {
                    PlayCustomLevel(OnlineLevelBrowser::GetSelectedPath());
                }
                else
                {
                    browsing = false;
                }
            }
            continue;  // Return to main level select
        }

        // Normal game loop for built-in level
        Game::isCustomLevel = false;
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

                // Keep music streaming buffer filled
                MusicStream::Update();

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
