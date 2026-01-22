#include "Game.hpp"

#include "GridMesh.hpp"
#include "GameLevelLoader.hpp"

// Screen bounds for the grid (trapezoid shape due to perspective)
// Top edge of grid (further from camera, appears narrower)
constexpr int GRID_TOP_LEFT = 90;
constexpr int GRID_TOP_RIGHT = 195;
constexpr int GRID_SCREEN_TOP = 75;

// Bottom edge of grid (closer to camera, appears wider)
constexpr int GRID_BOTTOM_LEFT = 70;
constexpr int GRID_BOTTOM_RIGHT = 216;
constexpr int GRID_SCREEN_BOTTOM = 130;

// Minimum drag distance to trigger movement (in pixels)
constexpr int DRAG_THRESHOLD = 20;

Grid2D Game::ScreenToGrid(int px, int py)
{
    // Clamp Y to grid bounds
    if (py < GRID_SCREEN_TOP) py = GRID_SCREEN_TOP;
    if (py > GRID_SCREEN_BOTTOM) py = GRID_SCREEN_BOTTOM;

    // Calculate Y position as ratio (0.0 = top, 1.0 = bottom)
    float yRatio = static_cast<float>(py - GRID_SCREEN_TOP) / (GRID_SCREEN_BOTTOM - GRID_SCREEN_TOP);

    // Interpolate left and right bounds based on Y position (trapezoid)
    int leftBound = GRID_TOP_LEFT + static_cast<int>(yRatio * (GRID_BOTTOM_LEFT - GRID_TOP_LEFT));
    int rightBound = GRID_TOP_RIGHT + static_cast<int>(yRatio * (GRID_BOTTOM_RIGHT - GRID_TOP_RIGHT));

    // Clamp X to interpolated bounds
    if (px < leftBound) px = leftBound;
    if (px > rightBound) px = rightBound;

    // Map to grid coordinates (0-5)
    int gridX = (px - leftBound) * 6 / (rightBound - leftBound);
    // Add 1 to compensate for perspective (touch registers one row higher than visual)
    int gridY = static_cast<int>(yRatio * 6) + 1;

    // Clamp to valid grid range
    if (gridX < 0) gridX = 0;
    if (gridX > 5) gridX = 5;
    if (gridY < 0) gridY = 0;
    if (gridY > 5) gridY = 5;

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

        // Car also occupies a second cell based on orientation
        if (PosVehicules::OrientationRULESpreset.at(car.orientation) == OrientationRULES::LEFT_RIGHT)
        {
            // Car extends to the right
            Grid2D secondCell = {static_cast<uint8_t>(car.grid2d.x + 1), car.grid2d.y};
            if (secondCell == grid)
                return static_cast<int>(i);
        }
        else
        {
            // Car extends downward
            Grid2D secondCell = {car.grid2d.x, static_cast<uint8_t>(car.grid2d.y + 1)};
            if (secondCell == grid)
                return static_cast<int>(i);
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

        // Update edit_car to show selection highlight
        if (touch_selected_car >= 0)
        {
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

        // Check if drag exceeds threshold
        if (carOrientation == OrientationRULES::LEFT_RIGHT)
        {
            // Horizontal movement only
            if (deltaX > DRAG_THRESHOLD && car.grid2d.x <= 3)
            {
                // Drag right
                car.grid2d.x += 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.x -= 1;
                }
                else
                {
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
                    touch_last_x = touch.px;
                    touch_last_y = touch.py;
                }
            }
        }
        else
        {
            // Vertical movement only (TOP_UP)
            if (deltaY > DRAG_THRESHOLD && car.grid2d.y <= 3)
            {
                // Drag down (increases Y in grid, which moves car down on screen)
                car.grid2d.y += 1;
                if (GameLevelLoader::CollisionCheck(car.grid2d, touch_selected_car))
                {
                    car.grid2d.y -= 1;
                }
                else
                {
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

void Game::Init()
{


    idMesh = 0;
    idOrient = 0;
    idTex = 0;

    edit_car = 0;
    //FILE* file;

    nitroFSInit(NULL);

    videoSetMode(MODE_0_3D);
    lcdMainOnBottom();
    
    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_OUTLINE);

    glSetOutlineColor(0, RGB15(31, 0, 0));

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // alpha blending works.
    GridMesh::LoadGridMesh(&grid);

    size_t mesh = 0;
    size_t ori = 0;
    size_t tex = 0;

    GameLevelLoader::LoadLevel(0);
    //cars.at(0) = {.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, .carID = mesh, .orientation = ori, .tex = tex, .basepose = PosVehicules::BasePoses.at(ori), .grid2d = {0, 0}};

    

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

    glClearColor(0, 0, 0, 31);
    glClearPolyID(0);
    //glClearPolyID(63);

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
    Game::HandleTouch();
    
    if (keys & KEY_LEFT)
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

    if (keys & KEY_RIGHT)
    {
        if (GameLevelLoader::lev_data.at(edit_car).grid2d.x <= 3 && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::LEFT_RIGHT)
        {
            GameLevelLoader::lev_data.at(edit_car).grid2d.x += 1;
            if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
            {
                GameLevelLoader::lev_data.at(edit_car).grid2d.x -= 1;
            }
        }
    }
    
    if (keys & KEY_UP)
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

    if (keys & KEY_DOWN)
    {
        if (GameLevelLoader::lev_data.at(edit_car).grid2d.y <= 3 && PosVehicules::OrientationRULESpreset.at(GameLevelLoader::lev_data.at(edit_car).orientation) == OrientationRULES::TOP_UP)
        {
            GameLevelLoader::lev_data.at(edit_car).grid2d.y += 1;
            if (GameLevelLoader::CollisionCheck(GameLevelLoader::lev_data.at(edit_car).grid2d, edit_car))
            {
                GameLevelLoader::lev_data.at(edit_car).grid2d.y -= 1;
            }
        }
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
    /*
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
    }*/

    if (keys & KEY_L)
    {
        edit_car++;
        if (GameLevelLoader::lev_data.at(edit_car).true_car == 0)
        {
            edit_car = 0;
        }
    }

    if (keys & KEY_R)
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
    


    
    glFlush(0);
    swiWaitForVBlank();
}