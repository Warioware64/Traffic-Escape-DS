#include "MainMenu.hpp"
#include "BGFont.hpp"
#include "SaveData.hpp"
#include "MusicStream.hpp"
#include "PeaberryBase_charmap.h"
#include "PeaberryBase_tiles_bin.h"
#include "PeaberryBase_pal_bin.h"
#include <cstring>
#include <cstdio>

// Level button layout constants
constexpr int BUTTON_START_X = 2;    // Starting X position (in characters)
constexpr int BUTTON_START_Y = 2;    // Starting Y position (in characters)
constexpr int BUTTONS_PER_ROW = 5;   // Number of buttons per row
constexpr int BUTTON_WIDTH = 5;      // Width of each button area (in characters)
constexpr int BUTTON_HEIGHT = 2;     // Height of each button area (in characters)

// Internal state for BG IDs
static int topBgId = -1;

// Helper function to load a background from nitrofiles
static bool LoadBGFromFile(const char* imgPath, const char* mapPath, const char* palPath,
                           int bgId, uint16_t* palette) {
    FILE* file;
    void* ptrImg = nullptr;
    void* ptrMap = nullptr;
    void* ptrPal = nullptr;
    size_t sizeImg = 0, sizeMap = 0, sizePal = 0;

    // Load image tiles
    file = fopen(imgPath, "rb");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    sizeImg = ftell(file);
    ptrImg = malloc(sizeImg);
    rewind(file);
    fread(ptrImg, 1, sizeImg, file);
    fclose(file);

    // Load map
    file = fopen(mapPath, "rb");
    if (!file) { free(ptrImg); return false; }
    fseek(file, 0, SEEK_END);
    sizeMap = ftell(file);
    ptrMap = malloc(sizeMap);
    rewind(file);
    fread(ptrMap, 1, sizeMap, file);
    fclose(file);

    // Load palette
    file = fopen(palPath, "rb");
    if (!file) { free(ptrImg); free(ptrMap); return false; }
    fseek(file, 0, SEEK_END);
    sizePal = ftell(file);
    ptrPal = malloc(sizePal);
    rewind(file);
    fread(ptrPal, 1, sizePal, file);
    fclose(file);

    // Copy to VRAM
    memcpy(bgGetGfxPtr(bgId), ptrImg, sizeImg);
    memcpy(bgGetMapPtr(bgId), ptrMap, sizeMap);
    memcpy(palette, ptrPal, sizePal);

    free(ptrImg);
    free(ptrMap);
    free(ptrPal);

    return true;
}

void MainMenu::TopBG()
{
    FILE* file;

    void *PtrImg;
    void *PtrMap;
    void *PtrPal;

    file = fopen("/BGs/topIntro.img.bin", "rb");
    if (!file) return;
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/topIntro.map.bin", "rb");
    if (!file) { free(PtrImg); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);

    file = fopen("/BGs/topIntro.pal.bin", "rb");
    if (!file) { free(PtrImg); free(PtrMap); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);
    int bg = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 2);
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE, PtrPal, size_bytes_pal);

    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}

void MainMenu::SubBG()
{
    FILE* file;

    void *PtrImg;
    void *PtrMap;
    void *PtrPal;

    file = fopen("/BGs/subIntro.img.bin", "rb");
    if (!file) return;
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/subIntro.map.bin", "rb");
    if (!file) { free(PtrImg); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);

    file = fopen("/BGs/subIntro.pal.bin", "rb");
    if (!file) { free(PtrImg); free(PtrMap); return; }
    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);
    int bg = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 2);
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE_SUB, PtrPal, size_bytes_pal);

    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}

void MainMenu::Intro()
{
    if (!nitroFSInit(NULL))
        exit(1);

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    TopBG();
    SubBG();

    int brightness = 16;
    for (int i = 0; i < 120 ; i++)
    {
        if (brightness > 0)
        {
            brightness--;
            setBrightness(3, brightness);
        }
        swiWaitForVBlank();
    }

    while (brightness < 16)
    {
        swiWaitForVBlank();
        brightness++;
        setBrightness(3, brightness);
    }

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
}

void MainMenu::Init()
{
    if (!nitroFSInit(NULL))
        exit(1);

    currentState = State::LEVEL_SELECT;
}

void MainMenu::LoadTopScreenBG()
{
    // Load TopScreen.png as main screen background
    topBgId = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(topBgId, 2);

    LoadBGFromFile("/BGs/TopScreen.img.bin", "/BGs/TopScreen.map.bin",
                   "/BGs/TopScreen.pal.bin", topBgId, BG_PALETTE);
}

void MainMenu::LoadSubScreenLevelSelect()
{
    // Keep screen blanked while loading (prevents magenta artifacts)
    setBrightness(3, 16);

    // Clear VRAM banks first (fixes melonDS showing old intro data)
    vramSetBankA(VRAM_A_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankH(VRAM_H_LCD);
    memset((void*)VRAM_A, 0, 128 * 1024);
    memset((void*)VRAM_C, 0, 128 * 1024);
    memset((void*)VRAM_H, 0, 32 * 1024);

    // Setup video mode
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    // Setup VRAM banks for use
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    // Enable extended palettes for sub screen
    bgExtPaletteEnableSub();

    // Load top screen background (main screen, no extended palette)
    LoadTopScreenBG();

    // Load sub screen background (TopScreen behind the font layer)
    int subBgId = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(subBgId, 2);  // Behind font layer

    // Load BG tiles and map
    FILE* file = fopen("/BGs/TopScreen.img.bin", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        void* ptr = malloc(size);
        rewind(file);
        fread(ptr, 1, size, file);
        fclose(file);
        memcpy(bgGetGfxPtr(subBgId), ptr, size);
        free(ptr);
    }

    file = fopen("/BGs/TopScreen.map.bin", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        void* ptr = malloc(size);
        rewind(file);
        fread(ptr, 1, size, file);
        fclose(file);
        memcpy(bgGetMapPtr(subBgId), ptr, size);
        free(ptr);
    }

    // Load palette to extended palette slot 1 (for BG1)
    file = fopen("/BGs/TopScreen.pal.bin", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        void* ptr = malloc(size);
        rewind(file);
        fread(ptr, 1, size, file);
        fclose(file);

        // Write to extended palette slot 1 (BG1)
        vramSetBankH(VRAM_H_LCD);  // Unlock for CPU access
        memcpy((uint8_t*)VRAM_H + 0x2000, ptr, size);  // Slot 1 at offset 0x2000
        vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);  // Re-enable for GPU
        free(ptr);
    }

    // Initialize BGFont on sub screen for level buttons (layer 0, in front)
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

    // Now that everything is loaded, show the screen
    setBrightness(3, 0);
}

void MainMenu::DrawLevelSelectUI()
{
    BGFont::Clear();

    // Title
    BGFont::Print(8, 0, "SELECT LEVEL");

    // Draw level buttons in a grid
    for (int i = 0; i < availableLevels && i < MAX_LEVELS; i++) {
        int row = i / BUTTONS_PER_ROW;
        int col = i % BUTTONS_PER_ROW;

        int x = BUTTON_START_X + col * BUTTON_WIDTH;
        int y = BUTTON_START_Y + row * BUTTON_HEIGHT;

        // Draw button with level number
        // Show * for completed levels
        if (SaveData::IsLevelCompleted(i)) {
            BGFont::Printf(x, y, "*%2d*", i + 1);
        } else {
            BGFont::Printf(x, y, "[%2d]", i + 1);
        }
    }

    // Instructions at bottom
    BGFont::Print(2, 10, "Touch a level to play");
}

int MainMenu::HandleLevelSelectTouch()
{
    scanKeys();

    if (keysDown() & KEY_TOUCH) {
        touchPosition touch;
        touchRead(&touch);

        // Convert touch position to character grid
        // Each character is 8 pixels wide, 16 pixels tall (for this font)
        int charX = touch.px / 8;
        int charY = touch.py / 16;

        // Check which button was pressed
        for (int i = 0; i < availableLevels && i < MAX_LEVELS; i++) {
            int row = i / BUTTONS_PER_ROW;
            int col = i % BUTTONS_PER_ROW;

            int btnX = BUTTON_START_X + col * BUTTON_WIDTH;
            int btnY = BUTTON_START_Y + row * BUTTON_HEIGHT;

            // Check if touch is within button bounds
            if (charX >= btnX && charX < btnX + 4 &&  // "[XX]" is 4 chars wide
                charY >= btnY && charY < btnY + 1) {
                return i;  // Return level index (0-based)
            }
        }
    }

    return -1;  // No selection
}

int MainMenu::ShowLevelSelect()
{
    LoadSubScreenLevelSelect();
    DrawLevelSelectUI();

    // Wait for level selection
    while (true) {
        swiWaitForVBlank();
        bgUpdate();
        MusicStream::Update();

        int selected = HandleLevelSelectTouch();
        if (selected >= 0) {
            selectedLevel = selected;

            // Visual feedback - highlight selection briefly
            int row = selected / BUTTONS_PER_ROW;
            int col = selected % BUTTONS_PER_ROW;
            int x = BUTTON_START_X + col * BUTTON_WIDTH;
            int y = BUTTON_START_Y + row * BUTTON_HEIGHT;
            BGFont::Printf(x, y, ">%2d<", selected + 1);

            // Brief delay for feedback
            for (int i = 0; i < 15; i++) {
                swiWaitForVBlank();
            }

            return selected;
        }

        // Check for B button to potentially go back (future use)
        if (keysDown() & KEY_B) {
            // Could return -1 for "go back" in future
        }
    }
}
