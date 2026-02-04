#ifndef PEABERRYBASE_CHARMAP_H
#define PEABERRYBASE_CHARMAP_H

#include <stdint.h>

// Font atlas metadata
#define PEABERRYBASE_TILE_WIDTH  8
#define PEABERRYBASE_TILE_HEIGHT 16
#define PEABERRYBASE_COLUMNS     16
#define PEABERRYBASE_CHAR_COUNT  95
#define PEABERRYBASE_FIRST_CHAR  32
#define PEABERRYBASE_LAST_CHAR   126

// Metatile dimensions in 8x8 base tiles
#define PEABERRYBASE_METATILE_W  1
#define PEABERRYBASE_METATILE_H  2

// Atlas layout info (for correct tile indexing)
#define PEABERRYBASE_TILES_PER_ROW  16  // 8x8 tiles per row in atlas

// Character width table for proportional spacing (in pixels)
static const uint8_t peaberrybase_char_widths[256] = {
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  6,  8,  8,  8,  8,  8,  8,  7,  7,  8,  8,  6,  8,  6,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  6,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  6,  8,  6,  8,  8,
     7,  8,  8,  8,  8,  8,  8,  8,  8,  7,  8,  8,  7,  8,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  5,  7,  8,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
};

// Get character advance width in pixels (for proportional spacing)
static inline uint8_t peaberrybase_get_width(char c) {
    return peaberrybase_char_widths[(uint8_t)c];
}

// Get base tile index for a character (top-left tile of the metatile)
static inline uint16_t peaberrybase_get_tile(char c) {
    if (c >= PEABERRYBASE_FIRST_CHAR && c <= PEABERRYBASE_LAST_CHAR) {
        int charIdx = c - PEABERRYBASE_FIRST_CHAR;
        int charCol = charIdx % PEABERRYBASE_COLUMNS;
        int charRow = charIdx / PEABERRYBASE_COLUMNS;
        // Each character row spans METATILE_H tile rows
        // Each character column spans METATILE_W tiles
        return (charRow * PEABERRYBASE_METATILE_H * PEABERRYBASE_TILES_PER_ROW) + (charCol * PEABERRYBASE_METATILE_W);
    }
    return 0;  // Default to first tile (space)
}

// Get tile index for a specific position within a character's metatile
// dx, dy are in 8x8 tile units (0 to METATILE_W-1, 0 to METATILE_H-1)
static inline uint16_t peaberrybase_get_metatile_idx(char c, int dx, int dy) {
    uint16_t base = peaberrybase_get_tile(c);
    return base + (dy * PEABERRYBASE_TILES_PER_ROW) + dx;
}

#endif // PEABERRYBASE_CHARMAP_H
