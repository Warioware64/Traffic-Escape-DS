#include "CustomLevelBrowser.hpp"
#include "GameLevelLoader.hpp"
#include "GridMesh.hpp"
#include "BGFont.hpp"
#include "MusicStream.hpp"
#include "soundbank.h"
#include "cJSON.h"

#include <nds.h>
#include <filesystem.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

// Maximum number of custom levels to scan
constexpr int MAX_CUSTOM_LEVELS = 64;

// Storage for scanned level file paths
static std::vector<std::string> levelPaths;
static std::vector<std::string> levelNames;
static std::vector<std::string> levelAuthors;
static int currentIndex = 0;
static char selectedPath[256] = {0};

// 3D preview state
static void* previewGrid = nullptr;

// Scan directory for JSON level files
static int ScanLevels()
{
    levelPaths.clear();
    levelNames.clear();
    levelAuthors.clear();

    // Try sd:/ first, then fat:/
    const char* paths[] = {"sd:/TE_LEVELS", "fat:/TE_LEVELS"};

    DIR* dir = nullptr;
    const char* basePath = nullptr;

    for (auto& p : paths)
    {
        dir = opendir(p);
        if (dir)
        {
            basePath = p;
            break;
        }
    }

    if (!dir)
        return 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (levelPaths.size() >= MAX_CUSTOM_LEVELS)
            break;

        // Check for .json extension
        const char* name = entry->d_name;
        size_t len = strlen(name);
        if (len < 6) continue;  // minimum: "x.json"
        if (strcasecmp(name + len - 5, ".json") != 0)
            continue;

        // Build full path
        std::string fullPath = std::string(basePath) + "/" + name;

        // Try to read level name and author from JSON
        std::string levelName = name;  // fallback to filename
        std::string author = "Unknown";

        FILE* f = fopen(fullPath.c_str(), "rb");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            rewind(f);

            // Only parse metadata, limit read size for safety
            if (size < 32768)
            {
                char* buf = (char*)malloc(size + 1);
                if (buf)
                {
                    fread(buf, 1, size, f);
                    buf[size] = '\0';

                    cJSON* j = cJSON_Parse(buf);
                    free(buf);

                    if (!j)
                    {
                        fclose(f);
                        continue;
                    }

                    cJSON* nameItem = cJSON_GetObjectItem(j, "level_name");
                    if (nameItem && cJSON_IsString(nameItem))
                        levelName = nameItem->valuestring;
                    cJSON* authorItem = cJSON_GetObjectItem(j, "author");
                    if (authorItem && cJSON_IsString(authorItem))
                        author = authorItem->valuestring;
                    cJSON_Delete(j);
                }
            }
            fclose(f);
        }

        levelPaths.push_back(fullPath);
        levelNames.push_back(levelName);
        levelAuthors.push_back(author);
    }

    closedir(dir);
    return (int)levelPaths.size();
}

// Clean up preview resources
static void CleanupPreview()
{
    // Free car meshes
    for (auto& car : GameLevelLoader::lev_data)
    {
        if (car.ptrMesh != nullptr)
        {
            free(car.ptrMesh);
            car.ptrMesh = nullptr;
        }
        car.true_car = 0;
    }
    PosVehicules::ClearTextureCache();

    if (previewGrid != nullptr)
    {
        free(previewGrid);
        previewGrid = nullptr;
    }
}

// Free all browser heap allocations (level lists) to reclaim memory before game
static void CleanupBrowser()
{
    levelPaths.clear();
    levelPaths.shrink_to_fit();
    levelNames.clear();
    levelNames.shrink_to_fit();
    levelAuthors.clear();
    levelAuthors.shrink_to_fit();
}

// Load preview for current level
static void LoadPreview()
{
    CleanupPreview();

    if (currentIndex < 0 || currentIndex >= (int)levelPaths.size())
        return;

    // 3D engine already initialized in Show(), just reload assets

    // Load grid mesh
    GridMesh::LoadGridMesh(&previewGrid);

    // Load level data + sky BG + car meshes/textures
    GameLevelLoader::LoadLevelFromJSON(levelPaths[currentIndex].c_str());

    // Sky BG now in VRAM — make 3D clear transparent so it shows through
    glClearColor(0, 0, 0, 0);
}

// Render one frame of 3D preview
static void RenderPreview()
{
    // Render grid
    glPushMatrix();
    glTranslatef(-1.25, -0.25, 0.5);
    glRotateX(85);
    glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
    glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT1);
    glBindTexture(0, 0);
    if (previewGrid)
        glCallList(previewGrid);
    glPopMatrix(1);

    // Render cars
    for (const auto& car : GameLevelLoader::lev_data)
    {
        if (car.true_car != 0)
        {
            glPushMatrix();
            glTranslatef(car.basepose.x + (car.grid2d.x * 0.5f),
                        car.basepose.y + (car.grid2d.y * -0.5f),
                        car.basepose.z);
            glRotateX(85);
            glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
            glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);
            glBindTexture(0, car.texGLptr);
            glCallList(car.ptrMesh);
            glPopMatrix(1);
        }
    }

    glFlush(0);
}

// Draw browser UI on bottom screen
static void DrawBrowserUI()
{
    BGFont::Clear();

    if (levelPaths.empty())
    {
        BGFont::Print(4, 4, "No custom levels found!");
        BGFont::Print(2, 6, "Place .json files in");
        BGFont::Print(2, 7, "sd:/TE_LEVELS/");
        BGFont::Print(10, 11, "[ BACK ]");
        return;
    }

    BGFont::Print(5, 0, "CUSTOM LEVELS");
    BGFont::Printf(8, 1, "%d / %d", currentIndex + 1, (int)levelPaths.size());

    // Level name (truncate if too long)
    if (currentIndex < (int)levelNames.size())
    {
        const std::string& name = levelNames[currentIndex];
        if (name.length() > 28)
            BGFont::Printf(1, 3, "%.28s", name.c_str());
        else
            BGFont::Print(1, 3, name.c_str());
    }

    // Author
    if (currentIndex < (int)levelAuthors.size())
    {
        BGFont::Printf(1, 5, "by %s", levelAuthors[currentIndex].c_str());
    }

    // Navigation buttons
    BGFont::Print(2, 9, "[ PREV ]");
    BGFont::Print(12, 9, "[ PLAY ]");
    BGFont::Print(22, 9, "[ NEXT ]");
    BGFont::Print(10, 11, "[ BACK ]");
}

// Handle touch input, returns action
enum class BrowserAction {
    NONE,
    PREV,
    NEXT,
    PLAY,
    BACK
};

static BrowserAction HandleBrowserTouch()
{
    scanKeys();

    if (keysDown() & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);

        int charX = touch.px / 8;
        int charY = touch.py / 16;

        // PREV button (row 9, columns 2-9)
        if (charY == 9 && charX >= 2 && charX < 10)
            return BrowserAction::PREV;

        // PLAY button (row 9, columns 12-19)
        if (charY == 9 && charX >= 12 && charX < 20)
            return BrowserAction::PLAY;

        // NEXT button (row 9, columns 22-29)
        if (charY == 9 && charX >= 22 && charX < 30)
            return BrowserAction::NEXT;

        // BACK button (row 11, columns 10-17)
        if (charY == 11 && charX >= 10 && charX < 18)
            return BrowserAction::BACK;
    }

    return BrowserAction::NONE;
}

// Setup 3D engine on main screen (deferred until a preview is ready)
static bool glInitialized = false;

static void Setup3DEngine()
{
    setBrightness(3, 16);

    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankE(VRAM_E_MAIN_BG);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    glInit();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    glClearColor(0, 0, 0, 31);
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0,
              0.0, 1.0, 0.0,
              0.0, 1.0, 0.0);

    glMaterialf(GL_AMBIENT, RGB15(0, 0, 0));
    glMaterialf(GL_DIFFUSE, RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glLight(0, RGB15(31, 31, 31), floattov10(-1.0), floattov10(-1.0), floattov10(-1.0));
    glLight(1, RGB15(31, 31, 31), floattov10(-0.75), floattov10(-0.75), floattov10(-0.75));

    lcdMainOnTop();
    setBrightness(3, 0);

    glInitialized = true;
}

CustomLevelBrowser::Result CustomLevelBrowser::Show()
{
    // Keep previous screen content visible until a preview is ready
    // Sub screen (VRAM_C/H) not touched - persists from MainMenu

    glInitialized = false;

    // Scan for levels
    int count = ScanLevels();
    currentIndex = 0;

    // Load preview for first level (if any)
    if (count > 0)
    {
        Setup3DEngine();
        LoadPreview();
    }

    DrawBrowserUI();

    if (!glInitialized)
    {
        // No levels found — keep previous top screen, just swap for touch
        lcdMainOnTop();
    }

    // Main browser loop
    while (true)
    {
        swiWaitForVBlank();
        bgUpdate();
        MusicStream::Update();

        // Render 3D preview on top screen
        if (count > 0)
            RenderPreview();

        BrowserAction action = HandleBrowserTouch();

        switch (action)
        {
            case BrowserAction::PREV:
                if (count > 0)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    currentIndex--;
                    if (currentIndex < 0) currentIndex = count - 1;
                    LoadPreview();
                    DrawBrowserUI();
                }
                break;

            case BrowserAction::NEXT:
                if (count > 0)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    currentIndex++;
                    if (currentIndex >= count) currentIndex = 0;
                    LoadPreview();
                    DrawBrowserUI();
                }
                break;

            case BrowserAction::PLAY:
                if (count > 0)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    strncpy(selectedPath, levelPaths[currentIndex].c_str(), sizeof(selectedPath) - 1);
                    selectedPath[sizeof(selectedPath) - 1] = '\0';
                    CleanupPreview();
                    if (glInitialized) glDeinit();
                    CleanupBrowser();
                    return Result::PLAY;
                }
                break;

            case BrowserAction::BACK:
                MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                CleanupPreview();
                if (glInitialized) glDeinit();
                CleanupBrowser();
                return Result::BACK;

            default:
                break;
        }
    }
}

const char* CustomLevelBrowser::GetSelectedPath()
{
    return selectedPath;
}
