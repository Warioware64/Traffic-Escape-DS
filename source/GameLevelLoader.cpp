#include "GameLevelLoader.hpp"
#include "GameLevels.hpp"


inline bool carOccupiesCell(const CarsStates& car, Grid2D cell)
{
    // First cell the car occupies
    if (car.grid2d == cell)
        return true;

    bool isHorizontal = (PosVehicules::OrientationRULESpreset.at(car.orientation) == OrientationRULES::LEFT_RIGHT);

    // Second cell depends on orientation
    if (isHorizontal)
    {
        // Car extends to the right: also occupies (x+1, y)
        Grid2D secondCell = {.x = static_cast<uint8_t>(car.grid2d.x + 1), .y = car.grid2d.y};
        if (secondCell == cell)
            return true;

        // Third cell for 3-cell cars (carID 3 or 7)
        if (PosVehicules::GetCarSize(car.carID) == 3)
        {
            Grid2D thirdCell = {.x = static_cast<uint8_t>(car.grid2d.x + 2), .y = car.grid2d.y};
            if (thirdCell == cell)
                return true;
        }
    }
    else
    {
        // Car extends downward: also occupies (x, y+1)
        Grid2D secondCell = {.x = car.grid2d.x, .y = static_cast<uint8_t>(car.grid2d.y + 1)};
        if (secondCell == cell)
            return true;

        // Third cell for 3-cell cars (carID 3 or 7)
        if (PosVehicules::GetCarSize(car.carID) == 3)
        {
            Grid2D thirdCell = {.x = car.grid2d.x, .y = static_cast<uint8_t>(car.grid2d.y + 2)};
            if (thirdCell == cell)
                return true;
        }
    }

    return false;
}


void GameLevelLoader::LoadBGtoptext()
{
    FILE* file;
    
    void *PtrImg;
    void *PtrMap;
    void *PtrPal;
    
    file = fopen("/BGs/bgtopscreentext.img.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/bgtopscreentext.map.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);
    
    file = fopen("/BGs/bgtopscreentext.pal.bin", "rb");

    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);

    // Text layer on BG0 (in front), use map base 2, tile base 4 to avoid conflict
    int bg = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 2, 4);
    bgSetPriority(bg, 0);  // Highest priority (in front)
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);

    // Copy palette to extended palette slot 0 for BG0
    // Unlock VRAM_H for CPU access
    vramSetBankH(VRAM_H_LCD);
    memcpy(VRAM_H, PtrPal, size_bytes_pal);  // BG0 ext palette at offset 0
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    
    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}

void GameLevelLoader::LoadBGtop()
{
    FILE* file;
    
    void *PtrImg;
    void *PtrMap;
    void *PtrPal;
    
    file = fopen("/BGs/TopScreen.img.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/TopScreen.map.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);
    
    file = fopen("/BGs/TopScreen.pal.bin", "rb");

    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);

    // Background on BG1 (behind text), map base 0, tile base 1
    int bg = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 1);  // Behind text layer (priority 0)
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);

    // Copy palette to extended palette slot 0 for BG1 (offset 0x2000 = 8KB)
    vramSetBankH(VRAM_H_LCD);
    memcpy((uint8_t*)VRAM_H + 0x2000, PtrPal, size_bytes_pal);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    
    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}
void GameLevelLoader::LoadBG(size_t bgID)
{
    FILE* file;
    
    void *PtrImg;
    void *PtrMap;
    void *PtrPal;

    
    std::string pathImg = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".img.bin";
    std::string pathMap = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".map.bin";
    std::string pathPal = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".pal.bin";
    
    file = fopen(pathImg.c_str(), "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    pathImg.clear();

    file = fopen(pathMap.c_str(), "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);

    pathMap.clear();

    file = fopen(pathPal.c_str(), "rb");

    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);

    pathPal.clear();

    int bg = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 2);  // Lower priority (higher number) than 3D layer 0
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE, PtrPal, size_bytes_pal);
    
    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}

void GameLevelLoader::LoadLevel(int level)
{
    lev_data = Levels.at(level);

    std::for_each(lev_data.begin(), lev_data.end(), [](CarsStates& n){
        if (n.true_car == 1)
        {
            n.basepose = PosVehicules::BasePoses.at(n.orientation);

            // Fix weird behaviour of Car8 and Car4
            if (n.carID == 7 || n.carID == 3)
            {
                switch(n.orientation)
                {
                    case 1:
                    {
                        //n.basepose.y += 0.5;
                        n.basepose.x += 0.5;
                        break;
                    }
                    case 2:
                    {
                        n.basepose.y += 0.5;
                        break;
                    }

                    case 3:
                    {
                        n.basepose.y--;
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
            PosVehicules::LoadVehicule_Mesh(n.ptrMesh, n.carID, n.orientation);
            PosVehicules::LoadVehicule_Texture(&n.texGLptr, n.tex);
        }

    });
}

bool GameLevelLoader::LoadLevelFromFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
        return false;

    // Clear existing level data
    for (auto& car : lev_data)
    {
        car.true_car = 0;
        car.ptrMesh = nullptr;
        car.texGLptr = 0;
    }

    // Read header: number of cars (byte 0), background ID (byte 1)
    uint8_t numCars;
    uint8_t bgID;
    fread(&numCars, 1, 1, file);
    fread(&bgID, 1, 1, file);

    // Load the background
    LoadBG(bgID);
    LoadBGtop();
    LoadBGtoptext();
    // Read each car: true_car, carID, orientation, tex, grid_x, grid_y (6 bytes each)
    for (size_t i = 0; i < 16; i++)
    {
        uint8_t data[6];
        fread(data, 1, 6, file);

        lev_data[i].true_car = data[0];
        lev_data[i].carID = data[1];
        lev_data[i].orientation = data[2];
        lev_data[i].tex = data[3];
        lev_data[i].grid2d.x = data[4];
        lev_data[i].grid2d.y = data[5];
        lev_data[i].ptrMesh = nullptr;
        lev_data[i].texGLptr = 0;
    }

    fclose(file);

    // Initialize cars (same as LoadLevel)
    std::for_each(lev_data.begin(), lev_data.end(), [](CarsStates& n){
        if (n.true_car == 1)
        {
            n.basepose = PosVehicules::BasePoses.at(n.orientation);

            // Fix weird behaviour of Car8
            if (n.carID == 7 || n.carID == 3)
            {
                switch(n.orientation)
                {
                    case 1:
                        n.basepose.x += 0.5;
                        break;
                    case 2:

                        n.basepose.y -= 0.5;
                        break;
                    default:
                        break;
                }
            }
            PosVehicules::LoadVehicule_Mesh(n.ptrMesh, n.carID, n.orientation);
            PosVehicules::LoadVehicule_Texture(&n.texGLptr, n.tex);
        }
    });

    return true;
}

bool GameLevelLoader::CollisionCheck(Grid2D grid2d, size_t excludeIndex)
{
    const CarsStates& movingCar = lev_data.at(excludeIndex);
    bool isHorizontal = (PosVehicules::OrientationRULESpreset.at(movingCar.orientation) == OrientationRULES::LEFT_RIGHT);
    bool is3Cell = (PosVehicules::GetCarSize(movingCar.carID) == 3);

    // Get cells the moving car will occupy at the new position
    Grid2D cell1 = grid2d;
    Grid2D cell2;
    Grid2D cell3;

    if (isHorizontal)
    {
        cell2 = {.x = static_cast<uint8_t>(grid2d.x + 1), .y = grid2d.y};
        cell3 = {.x = static_cast<uint8_t>(grid2d.x + 2), .y = grid2d.y};
    }
    else
    {
        cell2 = {.x = grid2d.x, .y = static_cast<uint8_t>(grid2d.y + 1)};
        cell3 = {.x = grid2d.x, .y = static_cast<uint8_t>(grid2d.y + 2)};
    }

    size_t i = 0;
    auto it = std::find_if(lev_data.begin(), lev_data.end(), [&](const CarsStates& car){
        size_t currentIndex = i++;
        if (currentIndex == excludeIndex || car.true_car == 0)
            return false;

        // Check if any cell of the moving car collides with any cell of this car
        if (carOccupiesCell(car, cell1) || carOccupiesCell(car, cell2))
            return true;

        // Check third cell for 3-cell cars
        if (is3Cell && carOccupiesCell(car, cell3))
            return true;

        return false;
    });

    return it != lev_data.end();
}