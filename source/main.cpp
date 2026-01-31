


#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "Game.hpp"
#include "MainMenu.hpp"

int main(int argc, char *argv[])
{
    MainMenu::Intro();
    Game::Init();
    while (1)
    {
        Game::Update();
    }

    return 0;
}