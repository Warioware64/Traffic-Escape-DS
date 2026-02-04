#include "BGFont.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace BGFont {

// Internal state
static struct {
    int bgId;
    Screen screen;
    uint16_t* mapPtr;
    int mapWidth;       // Map width in 8x8 tiles
    int mapHeight;      // Map height in 8x8 tiles
    int charWidth;      // Character cell width in 8x8 tiles (metatile)
    int charHeight;     // Character cell height in 8x8 tiles (metatile)
    int tileWidthPx;    // Character cell width in pixels
    int tileHeightPx;   // Character cell height in pixels
    int screenWidthPx;  // Screen width in pixels (256)
    int screenHeightPx; // Screen height in pixels (192)
    int atlasColumns;   // Columns in the font atlas
    int tilesPerRow;    // 8x8 tiles per row in atlas image
    const uint8_t* charWidths; // Character width table (for proportional spacing)
    char firstChar;
    char lastChar;
    int paletteSlot;    // Current palette slot (for extended palettes)
    bool initialized;
} state = {0};

// Get tile index for a character in the atlas
// For 8x8 metatiles (1x1), this returns the single tile index
// For larger metatiles, this returns the top-left tile index
static inline int GetCharTileBase(char c) {
    if (c < state.firstChar || c > state.lastChar) {
        c = state.firstChar;  // Default to first char (usually space)
    }

    int charIndex = c - state.firstChar;
    int charRow = charIndex / state.atlasColumns;
    int charCol = charIndex % state.atlasColumns;

    // Calculate tile index in the atlas
    // Each character row spans charHeight tile rows in the atlas
    // Each character occupies charWidth tiles horizontally
    // tilesPerRow is the actual number of 8x8 tiles per row in the atlas image
    int tileIndex = (charRow * state.charHeight * state.tilesPerRow) + (charCol * state.charWidth);

    // Grit adds an empty tile at index 0, so all actual tiles are offset by 1
    return tileIndex + 1;
}

int Init(Screen screen, int bgLayer, const FontConfig& config,
         int mapBase, int tileBase, int priority, bool useExtPalette) {

    state.screen = screen;
    state.charWidth = config.tileWidth / 8;
    state.charHeight = config.tileHeight / 8;
    state.tileWidthPx = config.tileWidth;
    state.tileHeightPx = config.tileHeight;
    state.atlasColumns = config.columns;
    state.tilesPerRow = config.tilesPerRow;
    state.charWidths = config.charWidths;  // Can be NULL for fixed-width
    state.firstChar = config.firstChar;
    state.lastChar = config.lastChar;
    state.paletteSlot = 0;

    // Map dimensions (256x256 = 32x32 tiles)
    state.mapWidth = 32;
    state.mapHeight = 32;

    // Screen dimensions
    state.screenWidthPx = 256;
    state.screenHeightPx = 192;

    // Initialize background
    if (screen == SCREEN_MAIN) {
        state.bgId = bgInit(bgLayer, BgType_Text8bpp, BgSize_T_256x256, mapBase, tileBase);
    } else {
        state.bgId = bgInitSub(bgLayer, BgType_Text8bpp, BgSize_T_256x256, mapBase, tileBase);
    }

    if (state.bgId < 0) {
        return -1;
    }

    bgSetPriority(state.bgId, priority);
    state.mapPtr = bgGetMapPtr(state.bgId);

    // Copy tiles to VRAM
    memcpy(bgGetGfxPtr(state.bgId), config.tiles, config.tilesSize);

    // Copy palette
    if (useExtPalette) {
        // For extended palette, copy to the appropriate slot
        // This requires VRAM to be unlocked first
        if (screen == SCREEN_MAIN) {
            // Main screen extended palette
            vramSetBankE(VRAM_E_LCD);
            memcpy((uint8_t*)VRAM_E + (bgLayer * 0x2000), config.palette, config.paletteSize);
            vramSetBankE(VRAM_E_BG_EXT_PALETTE);
        } else {
            // Sub screen extended palette (VRAM_H)
            vramSetBankH(VRAM_H_LCD);
            memcpy((uint8_t*)VRAM_H + (bgLayer * 0x2000), config.palette, config.paletteSize);
            vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
        }
    } else {
        // Standard palette
        if (screen == SCREEN_MAIN) {
            memcpy(BG_PALETTE, config.palette, config.paletteSize);
        } else {
            memcpy(BG_PALETTE_SUB, config.palette, config.paletteSize);
        }
    }

    // Clear the map
    Clear();

    state.initialized = true;
    return state.bgId;
}

void Clear() {
    if (!state.initialized) return;

    // Fill map with first character (space) tile
    int firstTile = GetCharTileBase(state.firstChar);
    uint16_t mapEntry = firstTile | (state.paletteSlot << 12);

    for (int i = 0; i < state.mapWidth * state.mapHeight; i++) {
        state.mapPtr[i] = mapEntry;
    }
}

void ClearRegion(int x, int y, int width, int height) {
    if (!state.initialized) return;

    int firstTile = GetCharTileBase(state.firstChar);
    uint16_t mapEntry = firstTile | (state.paletteSlot << 12);

    // Convert character positions to tile positions
    int tileX = x * state.charWidth;
    int tileY = y * state.charHeight;
    int tileW = width * state.charWidth;
    int tileH = height * state.charHeight;

    for (int ty = tileY; ty < tileY + tileH && ty < state.mapHeight; ty++) {
        for (int tx = tileX; tx < tileX + tileW && tx < state.mapWidth; tx++) {
            state.mapPtr[ty * state.mapWidth + tx] = mapEntry;
        }
    }
}

void PutChar(int x, int y, char c) {
    if (!state.initialized) return;

    // Calculate screen dimensions in character units
    int screenCharsW = state.mapWidth / state.charWidth;
    int screenCharsH = state.mapHeight / state.charHeight;

    // Bounds check
    if (x < 0 || x >= screenCharsW || y < 0 || y >= screenCharsH) {
        return;
    }

    int baseTile = GetCharTileBase(c);

    // Convert character position to tile position
    int tileX = x * state.charWidth;
    int tileY = y * state.charHeight;

    // For metatiles, we need to set multiple map entries
    // The atlas tiles are arranged row by row, tilesPerRow is from the actual atlas image
    for (int dy = 0; dy < state.charHeight; dy++) {
        for (int dx = 0; dx < state.charWidth; dx++) {
            int tileIndex = baseTile + (dy * state.tilesPerRow) + dx;
            uint16_t mapEntry = tileIndex | (state.paletteSlot << 12);

            int mapX = tileX + dx;
            int mapY = tileY + dy;

            if (mapX < state.mapWidth && mapY < state.mapHeight) {
                state.mapPtr[mapY * state.mapWidth + mapX] = mapEntry;
            }
        }
    }
}

void Print(int x, int y, const char* text) {
    if (!state.initialized || !text) return;

    // Calculate screen dimensions in character units
    int screenCharsW = state.mapWidth / state.charWidth;
    int screenCharsH = state.mapHeight / state.charHeight;

    int curX = x;
    int curY = y;

    while (*text) {
        char c = *text++;

        if (c == '\n') {
            curX = x;
            curY++;
            continue;
        }

        if (c == '\t') {
            curX = (curX + 4) & ~3;  // Align to next multiple of 4
            continue;
        }

        if (curX >= screenCharsW) {
            curX = x;
            curY++;
        }

        if (curY >= screenCharsH) {
            break;  // Out of screen
        }

        PutChar(curX, curY, c);
        curX++;
    }
}

void Printf(int x, int y, const char* fmt, ...) {
    if (!state.initialized || !fmt) return;

    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Print(x, y, buffer);
}

void SetPaletteSlot(int slot) {
    state.paletteSlot = slot & 0xF;  // Palette slot is 4 bits
}

int GetScreenWidth() {
    // Return width in 8-pixel units (for x positioning in Print)
    return state.screenWidthPx / 8;
}

int GetScreenHeight() {
    // Return height in character row units
    return state.screenHeightPx / state.tileHeightPx;
}

uint16_t* GetMapPtr() {
    return state.mapPtr;
}

} // namespace BGFont
