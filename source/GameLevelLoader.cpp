#include "GameLevelLoader.hpp"
#include "GameLevels.hpp"
#include "cJSON.h"

static void printHeap(const char* label)
{
    size_t heapUsed = (uintptr_t)getHeapEnd() - (uintptr_t)getHeapStart();
    size_t heapFree = (uintptr_t)getHeapLimit() - (uintptr_t)getHeapEnd();
    fprintf(stderr, "[HEAP %s] used=%u free=%u\n", label, heapUsed, heapFree);
}


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
    if (!file) return;
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/bgtopscreentext.map.bin", "rb");
    if (!file) { free(PtrImg); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);
    
    file = fopen("/BGs/bgtopscreentext.pal.bin", "rb");
    if (!file) { free(PtrImg); free(PtrMap); return; }
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

    printHeap("LoadBGtop-start");

    file = fopen("/BGs/TopScreen.img.bin", "rb");
    if (!file) { fprintf(stderr, "[LoadBGtop] FAILED to open TopScreen.img.bin\n"); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/TopScreen.map.bin", "rb");
    if (!file) { free(PtrImg); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);

    file = fopen("/BGs/TopScreen.pal.bin", "rb");
    if (!file) { free(PtrImg); free(PtrMap); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);

    int bg = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    fprintf(stderr, "[LoadBGtop] bgInitSub returned %d, img=%zu map=%zu pal=%zu\n",
            bg, size_bytes_img, size_bytes_map, size_bytes_pal);
    bgSetPriority(bg, 1);
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);

    vramSetBankH(VRAM_H_LCD);
    memcpy((uint8_t*)VRAM_H + 0x2000, PtrPal, size_bytes_pal);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
    printHeap("LoadBGtop-done");
}
void GameLevelLoader::LoadBG(size_t bgID)
{
    FILE* file;

    void *PtrImg;
    void *PtrMap;
    void *PtrPal;

    printHeap("LoadBG-start");
    fprintf(stderr, "[LoadBG] bgID=%zu name=%s\n", bgID, BG_name_list.at(bgID).c_str());

    std::string pathImg = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".img.bin";
    std::string pathMap = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".map.bin";
    std::string pathPal = "/BGs/" + GameLevelLoader::BG_name_list.at(bgID) + ".pal.bin";

    file = fopen(pathImg.c_str(), "rb");
    if (!file) { fprintf(stderr, "[LoadBG] FAILED to open %s\n", pathImg.c_str()); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    if (!PtrImg) { fprintf(stderr, "[LoadBG] FAILED malloc img %zu\n", size_bytes_img); fclose(file); return; }
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    pathImg.clear();

    file = fopen(pathMap.c_str(), "rb");
    if (!file) { free(PtrImg); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);

    pathMap.clear();

    file = fopen(pathPal.c_str(), "rb");
    if (!file) { free(PtrImg); free(PtrMap); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);

    pathPal.clear();

    int bg = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    fprintf(stderr, "[LoadBG] bgInit returned %d, img=%zu map=%zu pal=%zu\n",
            bg, size_bytes_img, size_bytes_map, size_bytes_pal);
    fprintf(stderr, "[LoadBG] gfxPtr=%p mapPtr=%p\n", bgGetGfxPtr(bg), bgGetMapPtr(bg));
    bgSetPriority(bg, 2);
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE, PtrPal, size_bytes_pal);

    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
    printHeap("LoadBG-done");
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
    // LoadBGtoptext();  // Removed - not used
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

// Internal helper: parse JSON and populate lev_data, returns bgID or -1 on error
static int ParseJSONLevel(const char* jsonPath)
{
    printHeap("ParseJSON-start");

    FILE* file = fopen(jsonPath, "rb");
    if (!file)
    {
        fprintf(stderr, "[ParseJSON] FAILED fopen %s\n", jsonPath);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    fprintf(stderr, "[ParseJSON] fileSize=%zu\n", fileSize);

    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer)
    {
        fprintf(stderr, "[ParseJSON] FAILED malloc %zu bytes\n", fileSize + 1);
        fclose(file);
        return -1;
    }
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    printHeap("ParseJSON-before-parse");
    cJSON* j = cJSON_Parse(buffer);
    free(buffer);
    printHeap("ParseJSON-after-parse");

    if (!j)
    {
        fprintf(stderr, "[ParseJSON] cJSON_Parse failed\n");
        return -1;
    }

    // Clear existing level data
    for (auto& car : GameLevelLoader::lev_data)
    {
        car.true_car = 0;
        car.ptrMesh = nullptr;
        car.texGLptr = 0;
    }

    cJSON* bgItem = cJSON_GetObjectItem(j, "background_id");
    int bgID = 0;
    if (bgItem && cJSON_IsNumber(bgItem))
        bgID = bgItem->valueint;
    cJSON* cars = cJSON_GetObjectItem(j, "cars");
    if (cars && cJSON_IsArray(cars))
    {
        int count = cJSON_GetArraySize(cars);
        for (int i = 0; i < count && i < 16; i++)
        {
            cJSON* carJson = cJSON_GetArrayItem(cars, i);
            if (!carJson) break;

            cJSON* tmp;
            GameLevelLoader::lev_data[i].true_car = 1;

            tmp = cJSON_GetObjectItem(carJson, "car_id");
            GameLevelLoader::lev_data[i].carID = (tmp && cJSON_IsNumber(tmp)) ? tmp->valueint : 0;

            tmp = cJSON_GetObjectItem(carJson, "orientation");
            GameLevelLoader::lev_data[i].orientation = (tmp && cJSON_IsNumber(tmp)) ? tmp->valueint : 0;

            tmp = cJSON_GetObjectItem(carJson, "texture_id");
            GameLevelLoader::lev_data[i].tex = (tmp && cJSON_IsNumber(tmp)) ? tmp->valueint : 0;

            tmp = cJSON_GetObjectItem(carJson, "grid_x");
            GameLevelLoader::lev_data[i].grid2d.x = (tmp && cJSON_IsNumber(tmp)) ? tmp->valueint : 0;

            tmp = cJSON_GetObjectItem(carJson, "grid_y");
            GameLevelLoader::lev_data[i].grid2d.y = (tmp && cJSON_IsNumber(tmp)) ? tmp->valueint : 0;

            GameLevelLoader::lev_data[i].ptrMesh = nullptr;
            GameLevelLoader::lev_data[i].texGLptr = 0;
        }
    }

    cJSON_Delete(j);
    return bgID;
}

// Internal helper: initialize car basepose, meshes, textures (same post-processing as LoadLevelFromFile)
static void InitCarAssets()
{
    std::for_each(GameLevelLoader::lev_data.begin(), GameLevelLoader::lev_data.end(), [](CarsStates& n){
        if (n.true_car == 1)
        {
            n.basepose = PosVehicules::BasePoses.at(n.orientation);

            // Fix weird behaviour of Car8 and Car4
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
}

bool GameLevelLoader::LoadLevelFromJSON(const char* jsonPath)
{
    fprintf(stderr, "[LoadLevelFromJSON] path=%s\n", jsonPath);

    int bgID = ParseJSONLevel(jsonPath);
    fprintf(stderr, "[LoadLevelFromJSON] ParseJSONLevel returned bgID=%d\n", bgID);
    if (bgID < 0)
        return false;

    // Load main BG (sky background, changes per level)
    LoadBG(bgID);
    // Sub BG (TopScreen) is already in VRAM from MainMenu - just re-enable the hardware layer
    // (videoSetModeSub resets BG enable bits, so we need to re-register BG1)
    int subBg = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(subBg, 1);

    // Initialize car assets
    InitCarAssets();

    fprintf(stderr, "[LoadLevelFromJSON] Done\n");
    return true;
}

bool GameLevelLoader::LoadLevelDataFromJSON(const char* jsonPath)
{
    int bgID = ParseJSONLevel(jsonPath);
    if (bgID < 0)
        return false;

    // Data only - no BG loading, but still init car assets for 3D preview
    InitCarAssets();

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