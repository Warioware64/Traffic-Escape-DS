


#include <nds.h>
#include <stdio.h>

#include <filesystem.h>
#include "Game.hpp"


int main(int argc, char *argv[])
{
    
    Game::Init();
    while (1)
    {
        Game::Update();
    }

    return 0;
}