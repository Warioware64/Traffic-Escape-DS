#ifndef BGFONT_HPP
#define BGFONT_HPP

#include <nds.h>
#include <stdint.h>

// BG Font - Tile-based text rendering for Nintendo DS
// Uses a background layer to display text with custom fonts

namespace BGFont {

// Font configuration structure
struct FontConfig {
    const void* tiles;      // Pointer to tile data (8bpp)
    uint32_t tilesSize;     // Size of tile data in bytes
    const void* palette;    // Pointer to palette data
    uint32_t paletteSize;   // Size of palette data in bytes
    const uint8_t* charWidths; // Pointer to character width table (256 entries, in pixels)

    uint8_t tileWidth;      // Character cell width in pixels (must be multiple of 8)
    uint8_t tileHeight;     // Character cell height in pixels (must be multiple of 8)
    uint8_t columns;        // Number of columns in the font atlas
    uint8_t tilesPerRow;    // 8x8 tiles per row in atlas image (from _TILES_PER_ROW)
    char firstChar;         // First character in the font (usually ' ' = 32)
    char lastChar;          // Last character in the font (usually '~' = 126)
};

// Screen selection
enum Screen {
    SCREEN_MAIN,
    SCREEN_SUB
};

// Initialize BG font system on a specific screen and BG layer
// @param screen: SCREEN_MAIN or SCREEN_SUB
// @param bgLayer: BG layer to use (0-3)
// @param config: Font configuration
// @param mapBase: Map base (0-31)
// @param tileBase: Tile base (0-15)
// @param priority: BG priority (0=front, 3=back)
// @param useExtPalette: Use extended palette (requires VRAM setup)
// @return: BG ID on success, -1 on failure
int Init(Screen screen, int bgLayer, const FontConfig& config,
         int mapBase, int tileBase, int priority = 0, bool useExtPalette = false);

// Clear all text from the screen
void Clear();

// Clear a specific region
// @param x, y: Starting position in characters
// @param width, height: Size in characters
void ClearRegion(int x, int y, int width, int height);

// Print a string at position
// @param x, y: Position in characters (0-based)
// @param text: Null-terminated string to print
void Print(int x, int y, const char* text);

// Print a formatted string (printf-style)
// @param x, y: Position in characters
// @param fmt: Format string
void Printf(int x, int y, const char* fmt, ...);

// Print a single character
// @param x, y: Position in characters
// @param c: Character to print
void PutChar(int x, int y, char c);

// Set the palette slot for subsequent text (for colored text)
// @param slot: Palette slot (0-15 for extended palettes)
void SetPaletteSlot(int slot);

// Get current screen dimensions in characters
int GetScreenWidth();
int GetScreenHeight();

// Get the raw map pointer for advanced usage
uint16_t* GetMapPtr();

} // namespace BGFont

#endif // BGFONT_HPP
