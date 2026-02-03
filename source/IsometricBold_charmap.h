#ifndef ISOMETRICBOLD_CHARMAP_H
#define ISOMETRICBOLD_CHARMAP_H

#include <stdint.h>

// Font atlas metadata
#define ISOMETRICBOLD_TILE_WIDTH  24
#define ISOMETRICBOLD_TILE_HEIGHT 16
#define ISOMETRICBOLD_COLUMNS     8
#define ISOMETRICBOLD_CHAR_COUNT  95
#define ISOMETRICBOLD_FIRST_CHAR  32
#define ISOMETRICBOLD_LAST_CHAR   126

// Metatile dimensions in 8x8 base tiles
#define ISOMETRICBOLD_METATILE_W  3
#define ISOMETRICBOLD_METATILE_H  2

// Atlas layout info (for correct tile indexing)
#define ISOMETRICBOLD_TILES_PER_ROW  24  // 8x8 tiles per row in atlas

// Character width table for proportional spacing (in pixels)
static const uint8_t isometricbold_char_widths[256] = {
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8, 15, 10, 15, 15, 15, 15, 10, 15, 15, 10, 15, 10, 15, 10, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10, 10, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
};

// Get character advance width in pixels (for proportional spacing)
static inline uint8_t isometricbold_get_width(char c) {
    return isometricbold_char_widths[(uint8_t)c];
}

// Get base tile index for a character (top-left tile of the metatile)
static inline uint16_t isometricbold_get_tile(char c) {
    if (c >= ISOMETRICBOLD_FIRST_CHAR && c <= ISOMETRICBOLD_LAST_CHAR) {
        int charIdx = c - ISOMETRICBOLD_FIRST_CHAR;
        int charCol = charIdx % ISOMETRICBOLD_COLUMNS;
        int charRow = charIdx / ISOMETRICBOLD_COLUMNS;
        // Each character row spans METATILE_H tile rows
        // Each character column spans METATILE_W tiles
        return (charRow * ISOMETRICBOLD_METATILE_H * ISOMETRICBOLD_TILES_PER_ROW) + (charCol * ISOMETRICBOLD_METATILE_W);
    }
    return 0;  // Default to first tile (space)
}

// Get tile index for a specific position within a character's metatile
// dx, dy are in 8x8 tile units (0 to METATILE_W-1, 0 to METATILE_H-1)
static inline uint16_t isometricbold_get_metatile_idx(char c, int dx, int dy) {
    uint16_t base = isometricbold_get_tile(c);
    return base + (dy * ISOMETRICBOLD_TILES_PER_ROW) + dx;
}

#endif // ISOMETRICBOLD_CHARMAP_H
