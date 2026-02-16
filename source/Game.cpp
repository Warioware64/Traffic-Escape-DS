#include "Game.hpp"

#include "BGFont.hpp"
#include "SaveData.hpp"
#include "MusicStream.hpp"
#include "soundbank.h"

#include "PeaberryBase_charmap.h"  // Generated header
#include "PeaberryBase_tiles_bin.h"
#include "PeaberryBase_pal_bin.h"

#include "GridMesh.hpp"
#include "GameLevelLoader.hpp"

#include <cstring>

// Screen bounds for the grid (trapezoid shape due to perspective)
// Top edge of grid (further from camera, appears narrower)
constexpr int GRID_TOP_LEFT = 90;
constexpr int GRID_TOP_RIGHT = 195;
constexpr int GRID_SCREEN_TOP = 40;

// Bottom edge of grid (closer to camera, appears wider)
constexpr int GRID_BOTTOM_LEFT = 40;
constexpr int GRID_BOTTOM_RIGHT = 216;
constexpr int GRID_SCREEN_BOTTOM = 150;

// Minimum drag distance to trigger movement (in pixels)
constexpr int DRAG_THRESHOLD = 20;

// Enable/disable debug overlay
constexpr bool DEBUG_DRAW_TOUCH_ZONE = true;

// Convert screen pixel X (0-255) to GL coordinate (-1 to 1)
inline float screenToGLX(int px) {
    return (px / 128.0f) - 1.0f;
}

// Convert screen pixel Y (0-191) to GL coordinate (1 to -1, Y is flipped)
inline float screenToGLY(int py) {
    return 1.0f - (py / 96.0f);
}

inline float Conditional_x_touch_grid_first(int touch_x)
{
    if (touch_x > 80)
    {
        return 0.5;
    }
    else
    {
        return 0.0;
    }
}
void Game::DrawDebugTouchZone()
{
    if (!DEBUG_DRAW_TOUCH_ZONE) return;

    // Use the same base position as the grid board
    BasePos basepos = {-1.25f, 2.25f, 0.5f};

    // Draw small markers at each grid cell center in 3D space
    // Using the same transformation as cars
    for (int gridY = 0; gridY < 6; gridY++)
    {
        for (int gridX = 0; gridX < 6; gridX++)
        {
            glPushMatrix();

            // Same transform as cars: basepose + (grid2d.x * 0.5, grid2d.y * -0.5, z)
            glTranslatef(basepos.x + (gridX * 0.5f) + 0.25f,  // +0.25 to center in cell
                        basepos.y + (gridY * -0.5f) - 0.25f, // -0.25 to center in cell
                        basepos.z + 0.1f);  // Slightly above grid
            glRotateX(85);
            glScalef32(floattof32(0.1f), floattof32(0.1f), floattof32(0.1f));

            // Color based on grid position for easy identification
            // Corners are bright, center is dim
            int r = (gridX == 0 || gridX == 5) ? 255 : 100;
            int g = (gridY == 0 || gridY == 5) ? 255 : 100;
            int b = ((gridX + gridY) % 2 == 0) ? 255 : 50;
            glColor3b(r, g, b);

            glPolyFmt(POLY_ALPHA(25) | POLY_ID(60) | POLY_CULL_NONE);
            glBindTexture(0, 0);

            // Draw a small quad marker
            glBegin(GL_QUADS);
                glVertex3f(-1, -1, 0);
                glVertex3f( 1, -1, 0);
                glVertex3f( 1,  1, 0);
                glVertex3f(-1,  1, 0);
            glEnd();

            glPopMatrix(1);
        }
    }

    // Reset color
    glColor3b(255, 255, 255);
}

Grid2D Game::ScreenToGrid(int px, int py)
{
    // Clamp Y to grid bounds
    if (py < GRID_SCREEN_TOP) py = GRID_SCREEN_TOP;
    if (py > GRID_SCREEN_BOTTOM) py = GRID_SCREEN_BOTTOM;

    // Calculate Y position as ratio (0.0 = top, 1.0 = bottom)
    float yRatio = static_cast<float>(py - GRID_SCREEN_TOP) / (GRID_SCREEN_BOTTOM - GRID_SCREEN_TOP);

    // Interpolate left and right bounds based on Y position (trapezoid)
    float leftBound = static_cast<float>(GRID_TOP_LEFT) + (yRatio * (GRID_BOTTOM_LEFT - GRID_TOP_LEFT));
    float rightBound = static_cast<float>(GRID_TOP_RIGHT) + (yRatio * (GRID_BOTTOM_RIGHT - GRID_TOP_RIGHT));
    /*
    std::cout << "leftBound : " << leftBound << std::endl;
    std::cout << "rightBound : " << rightBound << std::endl;
    std::cout << "yRatio : " << yRatio << std::endl;
    */
    // Clamp X to interpolated bounds
    if (px < leftBound) px = leftBound;
    if (px > rightBound) px = rightBound;

    // Map to grid coordinates (0-5)

    int gridX = static_cast<int>((px - leftBound) * 6 / (rightBound - leftBound) + Conditional_x_touch_grid_first(px));
    // Add 1 to compensate for perspective (touch registers one row higher than visual)
    int gridY = static_cast<int>((yRatio * 6) + 0.5);
    
    // Clamp to valid grid range
    if (gridX < 0) gridX = 0;
    if (gridX > 5) gridX = 5;
    if (gridY < 0) gridY = 0;
    if (gridY > 5) gridY = 5;
    
    //std::cout << "gridX : " << gridX << " gridY : " << gridY << std::endl;
    return {static_cast<uint8_t>(gridX), static_cast<uint8_t>(gridY)};
}

int Game::FindCarAtGrid(Grid2D grid)
{
    for (size_t i = 0; i < GameLevelLoader::lev_data.size(); i++)
    {
        const CarsStates& car = GameLevelLoader::lev_data.at(i);
        if (car.true_car == 0)
            continue;

        // Check if this grid position is occupied by this car
        // Car occupies its base position
        if (car.grid2d == grid)
            return static_cast<int>(i);

        bool isHorizontal = (PosVehicules::OrientationRULESpreset.at(car.orientation) == OrientationRULES::LEFT_RIGHT);
        bool is3Cell = (PosVehicules::GetCarSize(car.carID) == 3);

        // Car also occupies a second cell based on orientation
        if (isHorizontal)
        {
            // Car extends to the right
            Grid2D secondCell = {static_cast<uint8_t>(car.grid2d.x + 1), car.grid2d.y};
            if (secondCell == grid)
                return static_cast<int>(i);

            // Third cell for 3-cell cars
            if (is3Cell)
            {
                Grid2D thirdCell = {static_cast<uint8_t>(car.grid2d.x + 2), car.grid2d.y};
                if (thirdCell == grid)
                    return static_cast<int>(i);
            }
        }
        else
        {
            // Car extends downward
            Grid2D secondCell = {car.grid2d.x, static_cast<uint8_t>(car.grid2d.y + 1)};
            if (secondCell == grid)
                return static_cast<int>(i);

            // Third cell for 3-cell cars
            if (is3Cell)
            {
                Grid2D thirdCell = {car.grid2d.x, static_cast<uint8_t>(car.grid2d.y + 2)};
                if (thirdCell == grid)
                    return static_cast<int>(i);
            }
        }
    }
    return -1; // No car found
}

void Game::HandleTouch()
{
    uint16_t keysD = keysDown();
    uint16_t keysH = keysHeld();
    uint16_t keysU = keysUp();

    touchPosition touch;
    touchRead(&touch);

    // Debug: print touch position and calculated grid (remove after calibration)
    /*if (keysH & KEY_TOUCH)
    {*/
    Grid2D debugGrid = ScreenToGrid(touch.px, touch.py);
    int foundCar = FindCarAtGrid(debugGrid);
    
    /*
    printf("\x1b[0;0Hpx=%3d py=%3d -> grid(%d,%d) car=%2d  \n",
            touch.px, touch.py, debugGrid.x, debugGrid.y, foundCar);
    */
    
    //}

    // Touch started
    if (keysD & KEY_TOUCH)
    {
        touch_start_x = touch.px;
        touch_start_y = touch.py;
        touch_last_x = touch.px;
        touch_last_y = touch.py;
        touch_dragging = false;

        // Find which car was touched
        Grid2D touchedGrid = ScreenToGrid(touch.px, touch.py);
        touch_selected_car = FindCarAtGrid(touchedGrid);

        // printf("\x1b[0;0Hpx=%3d py=%3d -> grid(%d,%d) car=%2d  \n",
        //     touch.px, touch.py, touchedGrid.x, touchedGrid.y, touch_selected_car);

        // Update edit_car to show selection highlight
        if (touch_selected_car >= 0)
        {
            MusicStream::PlaySFX(SFX_TOGGLE_ON);
            edit_car = touch_selected_car;
        }
    }

    // Touch held - check for drag movement
    if ((keysH & KEY_TOUCH) && touch_selected_car >= 0)
    {
        int deltaX = touch.px - touch_last_x;
        int deltaY = touch.py - touch_last_y;

        CarsStates& car = GameLevelLoader::lev_data.at(touch_selected_car);
        OrientationRULES carOrientation = PosVehicules::OrientationRULESpreset.at(car.orientation);

        uint8_t maxPos;

        if (touch_selected_car == 0)
        {
            maxPos = 7 - PosVehicules::GetCarSize(car.carID);
        } else {
            maxPos = 6 - PosVehicules::GetCarSize(car.carID); // 4 for 2-cell, 3 for 3-cell
        }
        

        // printf("\x1b[0;0Hpx=%3d py=%3d -> grid(%d,%d) car=%2d  \n",
        //     touch.px, touch.py, car.grid2d.x, car.grid2d.y, touch_selected_car);

        // Check if drag exceeds threshold
        if (carOrientation == OrientationRULES::LEFT_RIGHT)
        {
            // Horizontal movement only
            if (deltaX > DRAG_THRESHOLD && car.grid2d.x < maxPos)
            {
                // Drag right
                car.grid2d.x += 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.x -= 1;
                    fprintf(stderr, "[TOUCH] car%d drag RIGHT blocked by collision at x=%d\n",
                            touch_selected_car, car.grid2d.x + 1);
                }
                else
                {
                    MusicStream::PlaySFX(SFX_CLICK_DOUBLE_OFF);
                    fprintf(stderr, "[TOUCH] car%d moved RIGHT to x=%d (maxPos=%d)\n",
                            touch_selected_car, car.grid2d.x, maxPos);
                    touch_last_x = touch.px;
                    touch_last_y = touch.py;
                }
            }
            else if (deltaX < -DRAG_THRESHOLD && car.grid2d.x >= 1)
            {
                // Drag left
                car.grid2d.x -= 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.x += 1;
                }
                else
                {
                    MusicStream::PlaySFX(SFX_CLICK_DOUBLE_OFF);
                    touch_last_x = touch.px;
                    touch_last_y = touch.py;
                }
            }
        }
        else
        {
            // Vertical movement only (TOP_UP)
            if (deltaY > DRAG_THRESHOLD && car.grid2d.y < maxPos)
            {
                // Drag down (increases Y in grid, which moves car down on screen)
                car.grid2d.y += 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.y -= 1;
                }
                else
                {
                    MusicStream::PlaySFX(SFX_CLICK_DOUBLE_OFF);
                    touch_last_x = touch.px;
                    touch_last_y = touch.py;
                }
            }
            else if (deltaY < -DRAG_THRESHOLD && car.grid2d.y >= 1)
            {
                // Drag up (decreases Y in grid, which moves car up on screen)
                car.grid2d.y -= 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.y += 1;
                }
                else
                {
                    MusicStream::PlaySFX(SFX_CLICK_DOUBLE_OFF);
                    touch_last_x = touch.px;
                    touch_last_y = touch.py;
                }
            }
        }
    }

    // Touch released
    if (keysU & KEY_TOUCH)
    {
        touch_selected_car = -1;
        touch_dragging = false;
    }
}

bool Game::CheckVictory()
{
    // First car (index 0) is the player car that needs to escape
    const CarsStates& playerCar = GameLevelLoader::lev_data.at(0);

    // Player car must be active
    if (playerCar.true_car == 0)
        return false;

    // Player car must be horizontal (LEFT_RIGHT) to exit through the right
    if (PosVehicules::OrientationRULESpreset.at(playerCar.orientation) != OrientationRULES::LEFT_RIGHT)
        return false;

    // Check if car has reached the exit (max X position)
    // For a car of size N, max position is 6 - N (e.g., 4 for 2-cell car)
    uint8_t carSize = PosVehicules::GetCarSize(playerCar.carID);
    uint8_t exitPosition = 7 - carSize;

    // Only print when position changes to avoid spam
    static uint8_t lastX = 255;
    if (playerCar.grid2d.x != lastX)
    {
        fprintf(stderr, "[VICTORY?] car0 x=%d exitPos=%d carSize=%d orientation=%zu\n",
                playerCar.grid2d.x, exitPosition, carSize, playerCar.orientation);
        lastX = playerCar.grid2d.x;
    }

    // Victory when car reaches the exit position
    return playerCar.grid2d.x >= exitPosition;
}

void Game::Init(int level)
{
    // Blank screen during loading (prevents magenta artifacts)
    setBrightness(3, 16);

    // Debug console - outputs to emulator debug window (melonDS terminal / no$gba)
    consoleDebugInit(DebugDevice_NOCASH);
    fprintf(stderr, "[INIT] Game::Init level=%d\n", level);

    // Store current level
    currentLevel = level;

    // Reset game state
    gameState = GameState::PLAYING;
    timer_frames = 0;
    timer_running = true;
    isNewRecord = false;
    selectedOption = MenuOption::NONE;

    // Reset touch state
    touch_selected_car = -1;
    touch_dragging = false;

    idMesh = 0;
    idOrient = 0;
    idTex = 0;

    edit_car = 0;
    //FILE* file;

    //nitroFSInit(NULL);
    

    lcdMainOnBottom();
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    // VRAM layout (validated against GBATEK):
    // Bank A (128K): 3D texture slot 0 (needed for tex4x4 compressed)
    // Bank B (128K): 3D texture slot 1 (needed for tex4x4 compressed)
    // Bank C (128K): Sub engine BG at 0x6200000
    // Bank D (128K): Main engine BG at 0x6000000 (OFS=0) - 2D BG behind 3D
    // Bank F (16K):  3D texture palette slot 0
    // Bank H (32K):  Sub engine BG extended palette
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_MAIN_BG_0x06000000);

    vramSetBankF(VRAM_F_TEX_PALETTE);  // 3D texture palettes
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);  // Sub screen BG extended palettes

    // Enable extended palettes for sub screen
    bgExtPaletteEnableSub();

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_OUTLINE);

    glSetOutlineColor(0, RGB15(31, 0, 0));

    GridMesh::LoadGridMesh(&grid);

    size_t mesh = 0;
    size_t ori = 0;
    size_t tex = 0;

    // Load level based on selection
    char levelPath[32];
    sprintf(levelPath, "/Levels/Level%d.bin", currentLevel + 1);
    GameLevelLoader::LoadLevelFromFile(levelPath);
    //cars.at(0) = {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = mesh, .orientation = ori, .tex = tex, .basepose = PosVehicules::BasePoses.at(ori), .grid2d = {0, 0}};

    BGFont::FontConfig fontCfg = {
        .tiles = PeaberryBase_tiles_bin,
        .tilesSize = PeaberryBase_tiles_bin_size,
        .palette = PeaberryBase_pal_bin,
        .paletteSize = PeaberryBase_pal_bin_size,
        .charWidths = peaberrybase_char_widths,
        .tileWidth = PEABERRYBASE_TILE_WIDTH,
        .tileHeight = PEABERRYBASE_TILE_HEIGHT,
        .columns = PEABERRYBASE_COLUMNS,
        .tilesPerRow = PEABERRYBASE_TILES_PER_ROW,  // NEW: from charmap header
        .firstChar = ' ',
        .lastChar = '~'
    };

    BGFont::Init(BGFont::SCREEN_SUB, 0, fontCfg, 3, 5, 0, true);
    //x_test = PosVehicules::BasePoses.at(ori).x;
    //y_test = PosVehicules::BasePoses.at(ori).y;
    //z_test = PosVehicules::BasePoses.at(ori).z;
    /*
    glGenTextures(1, &textureID);

    glBindTexture(0, textureID);
    glTexImage2D(0, 0, GL_RGB256, 128, 128, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                     car_tex_bin);

    glColorTableEXT(0, 0, 256, 0, 0, car_pal_bin);*/

    // Alpha=0 so 2D background can be seen behind 3D layer
    glClearColor(0, 0, 0, 0);
    glClearPolyID(0);  // Must be different from polygon IDs for alpha blending

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

    int brightness = 16;
    for (int i = 0; i < 16 ; i++)
    {
        if (brightness > 0)
        {
            brightness--;
            setBrightness(3, brightness);
        }
        swiWaitForVBlank();
    }
}

void Game::InitCustom(const char* jsonPath)
{
    // Blank screen during loading (prevents magenta artifacts)
    setBrightness(3, 16);

    // Debug console - outputs to emulator debug window (melonDS terminal / no$gba)
    consoleDebugInit(DebugDevice_NOCASH);
    fprintf(stderr, "[INIT] Game::InitCustom path=%s\n", jsonPath);

    // Mark as custom level
    isCustomLevel = true;
    strncpy(customLevelPath, jsonPath, sizeof(customLevelPath) - 1);
    customLevelPath[sizeof(customLevelPath) - 1] = '\0';

    // Store current level
    currentLevel = -1;

    // Reset game state
    gameState = GameState::PLAYING;
    timer_frames = 0;
    timer_running = true;
    isNewRecord = false;
    selectedOption = MenuOption::NONE;

    // Reset touch state
    touch_selected_car = -1;
    touch_dragging = false;

    idMesh = 0;
    idOrient = 0;
    idTex = 0;

    edit_car = 0;

    lcdMainOnBottom();
    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

    // Clear only main BG VRAM (Bank D) - sub BG (Bank C) and ext palettes (Bank H)
    // are preserved since the sub background (TopScreen) never changes
    vramSetBankD(VRAM_D_LCD);
    memset((void*)VRAM_D, 0, 128 * 1024);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_MAIN_BG_0x06000000);

    vramSetBankF(VRAM_F_TEX_PALETTE);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    bgExtPaletteEnableSub();

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_OUTLINE);

    glSetOutlineColor(0, RGB15(31, 0, 0));

    GridMesh::LoadGridMesh(&grid);

    fprintf(stderr, "[InitCustom] VRAM: A=TEX B=TEX C=SUB_BG D=MAIN_BG F=TEX_PAL H=SUB_EXT_PAL\n");
    fprintf(stderr, "[InitCustom] heapUsed=%u heapFree=%u\n",
            (unsigned)((uintptr_t)getHeapEnd() - (uintptr_t)getHeapStart()),
            (unsigned)((uintptr_t)getHeapLimit() - (uintptr_t)getHeapEnd()));

    // Load custom level from JSON (calls LoadBG + LoadBGtop + InitCarAssets)
    bool loadOk = GameLevelLoader::LoadLevelFromJSON(jsonPath);
    fprintf(stderr, "[InitCustom] LoadLevelFromJSON=%d\n", loadOk);

    BGFont::FontConfig fontCfg = {
        .tiles = PeaberryBase_tiles_bin,
        .tilesSize = PeaberryBase_tiles_bin_size,
        .palette = PeaberryBase_pal_bin,
        .paletteSize = PeaberryBase_pal_bin_size,
        .charWidths = peaberrybase_char_widths,
        .tileWidth = PEABERRYBASE_TILE_WIDTH,
        .tileHeight = PEABERRYBASE_TILE_HEIGHT,
        .columns = PEABERRYBASE_COLUMNS,
        .tilesPerRow = PEABERRYBASE_TILES_PER_ROW,
        .firstChar = ' ',
        .lastChar = '~'
    };

    BGFont::Init(BGFont::SCREEN_SUB, 0, fontCfg, 3, 5, 0, true);

    // Alpha=0 so 2D background can be seen behind 3D layer
    glClearColor(0, 0, 0, 0);
    glClearPolyID(0);  // Must be different from polygon IDs for alpha blending

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, 256.0 / 192.0, 0.1, 100);

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

    int brightness = 16;
    for (int i = 0; i < 16 ; i++)
    {
        if (brightness > 0)
        {
            brightness--;
            setBrightness(3, brightness);
        }
        swiWaitForVBlank();
    }
}

void Game::Cleanup()
{
    // Flush any pending save data to disk
    SaveData::Flush();

    // Delete all car textures and free meshes
    for (auto& car : GameLevelLoader::lev_data)
    {
        if (car.texGLptr != 0)
        {
            car.texGLptr = 0;
        }
        if (car.ptrMesh != nullptr)
        {
            free(car.ptrMesh);
            car.ptrMesh = nullptr;
        }
        car.true_car = 0;
    }

    // Clear the texture cache (deletes all GL textures)
    PosVehicules::ClearTextureCache();

    // Free grid mesh if allocated
    if (grid != nullptr)
    {
        free(grid);
        grid = nullptr;
    }

    // Delete test texture if used
    if (textureID != 0)
    {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    // Free test car mesh
    if (car != nullptr)
    {
        free(car);
        car = nullptr;
    }
}

// Menu button positions (sub screen coordinates)
constexpr int MENU_BUTTON_X = 64;
constexpr int MENU_BUTTON_WIDTH = 128;
constexpr int MENU_BUTTON_HEIGHT = 24;
constexpr int MENU_BUTTON_SPACING = 32;

void Game::DrawPauseMenu()
{
    lcdMainOnTop();
    BGFont::Clear();
    BGFont::Print(11, 3, "PAUSED");

    // Draw menu options as text buttons
    BGFont::Print(11, 7, "Continue");
    BGFont::Print(12, 9, "Retry");
    BGFont::Print(12, 11, "Quit");
}

void Game::DrawVictoryMenu()
{
    lcdMainOnTop();
    BGFont::Clear();
    BGFont::Print(8, 2, "LEVEL COMPLETE!");

    if (isNewRecord) {
        BGFont::Print(9, 4, "NEW RECORD!");
    }

    // Show final time
    uint32_t total_seconds = timer_frames / 60;
    uint32_t remaining_frames = timer_frames % 60;
    uint32_t milliseconds = (remaining_frames * 1000) / 60;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    BGFont::Printf(7, 6, "Time: %02d:%02d.%03d", minutes, seconds, milliseconds);

    // Draw menu options (no "Next Level" for custom levels)
    if (!isCustomLevel && currentLevel < totalLevels - 1) {
        BGFont::Print(9, 9, "Next Level");
    }
    BGFont::Print(12, 11, "Retry");
    BGFont::Print(12, 13, "Quit");
}

Game::MenuOption Game::HandleMenuTouch(bool isPauseMenu)
{
    touchPosition touch;

    if (keysDown() & KEY_TOUCH)
    {
        touchRead(&touch);

        // Convert touch Y to text row (16 pixels per row - matches BGFont tile height)
        int touchRow = touch.py / 16;

        if (isPauseMenu)
        {
            // Pause menu: Continue at row 7, Retry at row 9, Quit at row 11
            if (touchRow >= 7 && touchRow <= 8) return MenuOption::CONTINUE;
            if (touchRow >= 9 && touchRow <= 10) return MenuOption::RETRY;
            if (touchRow >= 11) return MenuOption::QUIT;
        }
        else
        {
            // Victory menu: Next Level at row 9, Retry at row 11, Quit at row 13
            if (!isCustomLevel && currentLevel < totalLevels - 1)
            {
                if (touchRow >= 9 && touchRow <= 10) return MenuOption::NEXT_LEVEL;
                if (touchRow >= 11 && touchRow <= 12) return MenuOption::RETRY;
                if (touchRow >= 13) return MenuOption::QUIT;
            }
            else
            {
                // No next level available (or custom level) - Retry at row 11, Quit at row 13
                if (touchRow >= 11 && touchRow <= 12) return MenuOption::RETRY;
                if (touchRow >= 13) return MenuOption::QUIT;
            }
        }
    }

    return MenuOption::NONE;
}

Game::UpdateResult Game::Update()
{
    fprintf(stderr, "[UPDATE] Enter gameState=%d\n", (int)gameState);
    scanKeys();

    uint16_t keys = keysDown();
    uint16_t keysReleased = keysUp();
    bool change = false;
    bgUpdate();

    // Flag to prevent same-frame pause toggle
    static bool pauseInputConsumed = false;

    // State machine for game states
    static int dbg_frame = 0;
    dbg_frame++;
    if (dbg_frame % 60 == 0) // Print once per second to avoid spam
    {
        fprintf(stderr, "[STATE] frame=%d gameState=%d timer=%lu car0.x=%d car0.y=%d car0.active=%d\n",
                dbg_frame, (int)gameState, (unsigned long)timer_frames,
                GameLevelLoader::lev_data.at(0).grid2d.x,
                GameLevelLoader::lev_data.at(0).grid2d.y,
                GameLevelLoader::lev_data.at(0).true_car);
    }

    switch (gameState)
    {
        case GameState::PAUSED:
        {
            DrawPauseMenu();

            // Handle touch input for menu
            MenuOption option = HandleMenuTouch(true);

            if (option != MenuOption::NONE)
                MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);

            // Check for START press to resume (with one-frame delay to prevent instant toggle)
            if (!pauseInputConsumed && (keys & KEY_START))
            {
                gameState = GameState::PLAYING;
                timer_running = true;
                BGFont::Clear();
                pauseInputConsumed = true;
            }
            else if (option == MenuOption::CONTINUE)
            {
                gameState = GameState::PLAYING;
                timer_running = true;
                lcdMainOnBottom();
                BGFont::Clear();
            }
            else if (option == MenuOption::RETRY)
            {
                Cleanup();
                return UpdateResult::RETRY_LEVEL;
            }
            else if (option == MenuOption::QUIT)
            {
                Cleanup();
                return UpdateResult::QUIT_TO_MENU;
            }

            // Clear consumed flag when START is released
            if (!(keysHeld() & KEY_START))
            {
                pauseInputConsumed = false;
            }
            break;
        }

        case GameState::VICTORY:
        {
            fprintf(stderr, "[VICTORY MENU] Drawing victory menu\n");
            DrawVictoryMenu();

            MenuOption option = HandleMenuTouch(false);
            if (option != MenuOption::NONE)
            {
                MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                fprintf(stderr, "[VICTORY MENU] Touch option=%d\n", (int)option);
            }
            if (option == MenuOption::NEXT_LEVEL)
            {
                Cleanup();
                return UpdateResult::NEXT_LEVEL;
            }
            else if (option == MenuOption::RETRY)
            {
                Cleanup();
                return UpdateResult::RETRY_LEVEL;
            }
            else if (option == MenuOption::QUIT)
            {
                Cleanup();
                return UpdateResult::QUIT_TO_MENU;
            }
            break;
        }

        case GameState::PLAYING:
        default:
        {
            // Update timer (60fps)
            if (timer_running) {
                timer_frames++;
            }

            // Calculate minutes, seconds, and milliseconds from frames (60fps)
            uint32_t total_seconds = timer_frames / 60;
            uint32_t remaining_frames = timer_frames % 60;
            uint32_t milliseconds = (remaining_frames * 1000) / 60;
            uint32_t minutes = total_seconds / 60;
            uint32_t seconds = total_seconds % 60;

            // Display current level
            if (isCustomLevel)
                BGFont::Print(9, 2, "Custom Level");
            else
                BGFont::Printf(11, 2, "Level %d", currentLevel + 1);
            // Display timer (MM:SS.mmm)
            BGFont::Printf(8, 4, "Time: %02d:%02d.%03d", minutes, seconds, milliseconds);

            // Display best time for current level (skip for custom)
            if (!isCustomLevel)
            {
                char bestTimeStr[16];
                SaveData::FormatTime(SaveData::GetBestTime(currentLevel), bestTimeStr, sizeof(bestTimeStr));
                BGFont::Printf(8, 6, "Best: %s", bestTimeStr);
            }

            // Detect victory
            if (CheckVictory())
            {
                fprintf(stderr, "[VICTORY!] Triggered! car0.x=%d timer=%lu\n",
                        GameLevelLoader::lev_data.at(0).grid2d.x, (unsigned long)timer_frames);
                MusicStream::PlaySFX(SFX_XYLOPHONE_LEVEL_COMPLETE);
                gameState = GameState::VICTORY;
                timer_running = false;
                // Only save best time for built-in levels
                if (!isCustomLevel)
                    isNewRecord = SaveData::SetBestTime(currentLevel, timer_frames);
                break;
            }

            // Check for START to pause (with flag to prevent instant toggle)
            if (!pauseInputConsumed && (keys & KEY_START))
            {
                gameState = GameState::PAUSED;
                timer_running = false;
                pauseInputConsumed = true;
                break;
            }

            // Clear consumed flag when START is released
            if (!(keysHeld() & KEY_START))
            {
                pauseInputConsumed = false;
            }

            // Normal game logic
            Game::HandleTouch();

            // Get max position for current car (4 for 2-cell, 3 for 3-cell)
            uint8_t editCarMaxPos = 6 - PosVehicules::GetCarSize(GameLevelLoader::lev_data.at(edit_car).carID);

            if (keysReleased & KEY_LEFT)
            {
                if (GameLevelLoader::lev_data.at(edit_car).grid2d.x >= 1 && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::LEFT_RIGHT)
                {
                    GameLevelLoader::lev_data.at(edit_car).grid2d.x -= 1;
                    if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
                    {
                        GameLevelLoader::lev_data.at(edit_car).grid2d.x += 1;
                    }
                }
            }

            if (keysReleased & KEY_RIGHT)
            {
                if (GameLevelLoader::lev_data.at(edit_car).grid2d.x < editCarMaxPos && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::LEFT_RIGHT)
                {
                    GameLevelLoader::lev_data.at(edit_car).grid2d.x += 1;
                    if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
                    {
                        GameLevelLoader::lev_data.at(edit_car).grid2d.x -= 1;
                    }
                }
            }

            if (keysReleased & KEY_UP)
            {
                if (GameLevelLoader::lev_data.at(edit_car).grid2d.y >= 1 && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::TOP_UP)
                {
                    GameLevelLoader::lev_data.at(edit_car).grid2d.y -= 1;
                    if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
                    {
                        GameLevelLoader::lev_data.at(edit_car).grid2d.y += 1;
                    }
                }
            }

            if (keysReleased & KEY_DOWN)
            {
                if (GameLevelLoader::lev_data.at(edit_car).grid2d.y < editCarMaxPos && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::TOP_UP)
                {
                    GameLevelLoader::lev_data.at(edit_car).grid2d.y += 1;
                    if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
                    {
                        GameLevelLoader::lev_data.at(edit_car).grid2d.y -= 1;
                    }
                }
            }
            break;
        }
    } // End of switch (gameState)

    if (keysReleased & KEY_A)
    {
        if (idMesh != PosVehicules::CarNames.size())
            idMesh++;
        change = true;
    }

    if (keysReleased & KEY_B)
    {
        if (idMesh != 0)
            idMesh--;
        change = true;
    }

    if (keysReleased & KEY_X)
    {
        if (idOrient != 3)
            idOrient++;
        change = true;
    }

    if (keysReleased & KEY_Y)
    {
        if (idOrient != 0)
            idOrient--;
        change = true;
    }

    if (keysReleased & KEY_L)
    {
        edit_car++;
        if (GameLevelLoader::lev_data.at(edit_car).true_car == 0)
        {
            edit_car = 0;
        }
    }

    if (keysReleased & KEY_R)
    {
        if (edit_car != 0)
            edit_car--;
    }
    if (change)
    {
        glDeleteTextures(1, &textureID);
        PosVehicules::LoadVehicule_Mesh(car, idMesh, idOrient);
        PosVehicules::LoadVehicule_Texture(&textureID, idTex);
        x_test = PosVehicules::BasePoses.at(idOrient).x;
        y_test = PosVehicules::BasePoses.at(idOrient).y;
        z_test = PosVehicules::BasePoses.at(idOrient).z;
    }
    //glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0);

    
    fprintf(stderr, "[RENDER] Start 3D render gameState=%d\n", (int)gameState);
    // Render grid
    glPushMatrix();
    glTranslatef(-1.25, -0.25, 0.5);
    glRotateX(85);
    glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
    glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT1);
    glBindTexture(0, 0);
    glCallList(grid);
    glPopMatrix(1);

    size_t itemCount = 0;

    std::for_each(GameLevelLoader::lev_data.begin(), GameLevelLoader::lev_data.end(), [&itemCount](const CarsStates& n){

        if (n.true_car != 0)
        {
            glPushMatrix();
            glTranslatef(n.basepose.x + (n.grid2d.x * 0.5), n.basepose.y + (n.grid2d.y * -0.5), n.basepose.z);
            glRotateX(85);

            glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
            if (itemCount == edit_car)
            {
                glPolyFmt(POLY_ALPHA(31) | POLY_ID(1) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);
            }
            else
            {
                glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);
            }

            glBindTexture(0, n.texGLptr);
            glCallList(n.ptrMesh);

            glPopMatrix(1);
        }

        itemCount++;
    });

    // Draw debug overlay showing touch zones
    //DrawDebugTouchZone();
    /*
    printf("getHeapStart : %d \n", getHeapStart());
    printf("getHeapLimit : %d \n", getHeapLimit());
    printf("getHeapEnd : %d \n", getHeapEnd());
    */
    /*
    std::cout << "getHeapStart : "<< (const char)getHeapStart() << std::endl;
    std::cout << "getHeapLimit : " << (const char)getHeapLimit() << std::endl;
    std::cout << "getHeapEnd : " << (const char)getHeapEnd() << std::endl;
    */
    /*
    while (GFX_STATUS & BIT(27));
            
    printf("%d Vertex\n", GFX_VERTEX_RAM_USAGE);
    printf("%d Polygon\n", GFX_POLYGON_RAM_USAGE);
    */

    fprintf(stderr, "[RENDER] Before glFlush\n");
    glFlush(0);
    fprintf(stderr, "[RENDER] After glFlush, before VBlank\n");
    swiWaitForVBlank();
    fprintf(stderr, "[RENDER] After VBlank\n");

    return UpdateResult::CONTINUE;
}