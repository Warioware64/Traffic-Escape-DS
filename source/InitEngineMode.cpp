#include "InitEngineMode.hpp"

void InitEngineMode::initialize()
{
    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    glClearColor(0, 0, 0, 0);
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);
}
