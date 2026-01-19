#include "Game.hpp"
#include "PosVehicules.hpp"
#include "GridMesh.hpp"

void Game::Init()
{
    x_test = -1.25;
    y_test = -0.25;
    z_test = 0.5;

    idMesh = 0;
    idOrient = 0;
    idTex = 0;
    //FILE* file;

    nitroFSInit(NULL);

    videoSetMode(MODE_0_3D);
    
    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // alpha blending works.
    GridMesh::LoadGridMesh(&grid);

    PosVehicules::LoadVehicule_Mesh(&car, 0, 0);
    PosVehicules::LoadVehicule_Texture(&textureID, 0);
    /*
    glGenTextures(1, &textureID);

    glBindTexture(0, textureID);
    glTexImage2D(0, 0, GL_RGB256, 128, 128, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                     car_tex_bin);

    glColorTableEXT(0, 0, 256, 0, 0, car_pal_bin);*/

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, 256.0 / 192.0, 0.1, 100);
    //glOrtho(-3, 3, -2, 2, 0.1, 100);

    
    // Initialization code for the game goes here
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, 4.0,  // Position
                0.0, 1.0, 0.0,  // Look at
                0.0, 1.0, 0.0); // Up
    
    // Setup some material properties
    glMaterialf(GL_AMBIENT, RGB15(0, 0, 0));
    glMaterialf(GL_DIFFUSE, RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Setup lights
    glLight(0, RGB15(31, 31, 31), floattov10(-1.0), floattov10(-1.0), floattov10(-1.0));
    glLight(1, RGB15(31, 31, 31), floattov10(-0.75), floattov10(-0.75), floattov10(-0.75));
}

void Game::Update()
{
    scanKeys();
    uint16_t keys = keysUp();
    bool change = false;

    if (keys & KEY_LEFT)
        x_test -= 0.5;
    if (keys & KEY_RIGHT)
        x_test += 0.5;
    
    
    if (keys & KEY_UP)
    {
        y_test += 0.5;
    }

    if (keys & KEY_DOWN)
    {
        y_test -= 0.5;
    }
    
    if (keys & KEY_A)
    {
        if (idMesh != PosVehicules::CarNames.size())
            idMesh++;
        change = true;
    }

    if (keys & KEY_B)
    {
        if (idMesh != 0)
            idMesh--;
        change = true;
    }

    if (keys & KEY_X)
    {
        if (idOrient != 3)
            idOrient++;
        change = true;
    }

    if (keys & KEY_Y)
    {
        if (idOrient != 0)
            idOrient--;
        change = true;
    }

    if (keys & KEY_R)
    {
        if (idTex != PosVehicules::TextureNames.size())
            idTex++;
        change = true;
    }

    if (keys & KEY_L)
    {
        if (idTex != 0)
            idTex--;
        change = true;
    }

    if (change)
    {
        glDeleteTextures(1, &textureID);
        PosVehicules::LoadVehicule_Mesh(&car, idMesh, idOrient);
        PosVehicules::LoadVehicule_Texture(&textureID, idTex);
        
    }
    //glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0);

    
    glPushMatrix();
    glTranslatef(-1.25, -0.25, 0.5);
    glRotateX(85);
    glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
    glPolyFmt(POLY_ALPHA(31) | POLY_ID(1) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT1);
    glBindTexture(0, 0);
    glCallList(grid);

    glPopMatrix(1);
    



    glPushMatrix();
    glTranslatef(x_test, y_test, z_test);
    glRotateX(85);

    glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));

    glPolyFmt(POLY_ALPHA(31) | POLY_ID(2) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);

    glBindTexture(0, textureID);
    glCallList(car);

    glPopMatrix(1);
    
    glFlush(0);
    swiWaitForVBlank();
}