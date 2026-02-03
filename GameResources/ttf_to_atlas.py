#!/usr/bin/env python3
"""
TTF to NDS Font Atlas Converter

Generates a font atlas PNG from a TTF file, suitable for grit conversion
to NDS background tiles. Also generates a C header with character mappings.

Usage:
    python ttf_to_atlas.py font.ttf -o output_name [options]

Options:
    --size SIZE         Font size in pixels (default: 8)
    --metatile WxH      Metatile size, e.g., 8x8, 8x16, 16x16 (default: 8x8)
    --chars CHARS       Character range: 'ascii', 'extended', or custom string
    --columns COLS      Number of columns in atlas (default: 16)
    --transparent COLOR Transparent color in hex (default: FF00FF)
    --antialias         Enable antialiasing (not recommended for NDS)
"""

import argparse
import os
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow")
    sys.exit(1)


def parse_metatile(metatile_str):
    """Parse metatile string like '8x8' or '8x16' into (width, height)"""
    try:
        w, h = metatile_str.lower().split('x')
        return int(w), int(h)
    except ValueError:
        raise ValueError(f"Invalid metatile format: {metatile_str}. Use WxH (e.g., 8x8)")


def get_character_set(chars_option):
    """Get the character set based on option"""
    if chars_option == 'ascii':
        # Printable ASCII: space (32) to tilde (126)
        return ''.join(chr(i) for i in range(32, 127))
    elif chars_option == 'extended':
        # Extended ASCII including common accented characters
        return ''.join(chr(i) for i in range(32, 256) if chr(i).isprintable())
    elif chars_option == 'basic':
        # Minimal set: digits, uppercase, lowercase, common punctuation
        return ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'
    else:
        # Custom character string
        return chars_option


def hex_to_rgb(hex_color):
    """Convert hex color string to RGB tuple"""
    hex_color = hex_color.lstrip('#')
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))


def create_font_atlas(ttf_path, output_name, font_size=8, metatile=(8, 8),
                      chars='ascii', columns=16, transparent_color='FF00FF',
                      antialias=False):
    """
    Create a font atlas from a TTF file.

    Args:
        ttf_path: Path to TTF font file
        output_name: Base name for output files (without extension)
        font_size: Font size in pixels
        metatile: Tuple of (width, height) for each character cell
        chars: Character set to include
        columns: Number of columns in the atlas
        transparent_color: Hex color for transparency (magenta default)
        antialias: Whether to use antialiasing
    """
    tile_w, tile_h = metatile
    trans_rgb = hex_to_rgb(transparent_color)

    # Get character set
    char_set = get_character_set(chars)
    num_chars = len(char_set)

    # Calculate atlas dimensions
    rows = (num_chars + columns - 1) // columns
    atlas_width = columns * tile_w
    atlas_height = rows * tile_h

    # For tiled backgrounds, dimensions must be multiples of 8 (not necessarily power of 2)
    # Round up to nearest multiple of 8
    def round_to_8(n):
        return ((n + 7) // 8) * 8

    atlas_width = round_to_8(atlas_width)
    atlas_height = round_to_8(atlas_height)

    # Store actual tile dimensions for the header
    tiles_per_row = atlas_width // 8  # Actual 8x8 tiles per row in atlas

    # Calculate total tiles for VRAM estimation
    total_tiles = (atlas_width // 8) * (atlas_height // 8)
    vram_size = total_tiles * 64  # 64 bytes per 8bpp tile

    print(f"Font: {ttf_path}")
    print(f"Characters: {num_chars}")
    print(f"Metatile: {tile_w}x{tile_h} ({tile_w//8}x{tile_h//8} tiles)")
    print(f"Atlas size: {atlas_width}x{atlas_height} pixels")
    print(f"Grid: {columns} columns x {rows} rows")
    print(f"Tiles per row: {tiles_per_row}")
    print(f"Total tiles: {total_tiles} ({vram_size} bytes / {vram_size/1024:.1f} KB)")

    # Warn about inefficient configurations
    if columns == 1 and num_chars > 32:
        print(f"\nWARNING: Using columns=1 creates a very tall atlas ({atlas_height}px).")
        print(f"         Consider using --columns 8 or --columns 16 for better VRAM efficiency.")

    # Create image with transparent color background
    atlas = Image.new('RGB', (atlas_width, atlas_height), trans_rgb)
    draw = ImageDraw.Draw(atlas)

    # Load font
    try:
        font = ImageFont.truetype(ttf_path, font_size)
    except IOError:
        print(f"Error: Could not load font: {ttf_path}")
        sys.exit(1)

    # Character to tile index mapping and width tracking
    char_map = {}
    char_widths = {}  # Track actual rendered width of each character

    # Render each character
    for i, char in enumerate(char_set):
        col = i % columns
        row = i // columns

        x = col * tile_w
        y = row * tile_h

        # Get character bounding box for centering vertically, but LEFT-ALIGN horizontally
        bbox = font.getbbox(char)
        if bbox:
            char_w = bbox[2] - bbox[0]
            char_h = bbox[3] - bbox[1]

            # Left-align character (small padding), center vertically
            offset_x = 1 - bbox[0]  # 1 pixel left padding
            offset_y = (tile_h - char_h) // 2 - bbox[1]

            # Store actual advance width (glyph width + 1 pixel spacing)
            char_widths[char] = min(char_w + 2, tile_w)  # Cap at tile width
        else:
            offset_x = 0
            offset_y = 0
            char_widths[char] = tile_w // 2  # Default for space/empty

        # Special case for space character
        if char == ' ':
            char_widths[char] = tile_w // 3  # Space is narrower

        # Draw character
        text_color = (255, 255, 255)  # White text
        draw.text((x + offset_x, y + offset_y), char, font=font, fill=text_color)

        # Store mapping
        char_map[char] = i

    # Save atlas PNG
    png_path = f"{output_name}.png"
    atlas.save(png_path)
    print(f"Saved atlas: {png_path}")

    # Generate C header with character mappings and widths
    header_path = f"{output_name}_charmap.h"
    generate_header(header_path, output_name, char_set, char_map, char_widths, tile_w, tile_h, columns, tiles_per_row)
    print(f"Saved header: {header_path}")

    # Generate grit conversion script
    script_path = f"{output_name}_convert.sh"
    generate_grit_script(script_path, output_name, transparent_color)
    print(f"Saved script: {script_path}")

    return atlas, char_map


def generate_header(path, name, char_set, char_map, char_widths, tile_w, tile_h, columns, tiles_per_row):
    """Generate C header file with font metadata and character mappings"""

    # Sanitize name for C identifiers
    c_name = name.replace('-', '_').replace(' ', '_').upper()

    metatile_w = tile_w // 8
    metatile_h = tile_h // 8
    tiles_per_char = metatile_w * metatile_h

    # Generate width table (256 entries for direct ASCII lookup)
    width_table = [tile_w // 3] * 256  # Default width for unknown chars
    for char, width in char_widths.items():
        width_table[ord(char)] = width

    header = f"""#ifndef {c_name}_CHARMAP_H
#define {c_name}_CHARMAP_H

#include <stdint.h>

// Font atlas metadata
#define {c_name}_TILE_WIDTH  {tile_w}
#define {c_name}_TILE_HEIGHT {tile_h}
#define {c_name}_COLUMNS     {columns}
#define {c_name}_CHAR_COUNT  {len(char_set)}
#define {c_name}_FIRST_CHAR  {ord(char_set[0])}
#define {c_name}_LAST_CHAR   {ord(char_set[-1])}

// Metatile dimensions in 8x8 base tiles
#define {c_name}_METATILE_W  {metatile_w}
#define {c_name}_METATILE_H  {metatile_h}

// Atlas layout info (for correct tile indexing)
#define {c_name}_TILES_PER_ROW  {tiles_per_row}  // 8x8 tiles per row in atlas

// Character width table for proportional spacing (in pixels)
static const uint8_t {c_name.lower()}_char_widths[256] = {{
"""

    # Add width table data
    for i in range(0, 256, 16):
        values = width_table[i:i+16]
        header += "    " + ", ".join(f"{v:2d}" for v in values) + ",\n"

    header += f"""}};

// Get character advance width in pixels (for proportional spacing)
static inline uint8_t {c_name.lower()}_get_width(char c) {{
    return {c_name.lower()}_char_widths[(uint8_t)c];
}}

// Get base tile index for a character (top-left tile of the metatile)
static inline uint16_t {c_name.lower()}_get_tile(char c) {{
    if (c >= {c_name}_FIRST_CHAR && c <= {c_name}_LAST_CHAR) {{
        int charIdx = c - {c_name}_FIRST_CHAR;
        int charCol = charIdx % {c_name}_COLUMNS;
        int charRow = charIdx / {c_name}_COLUMNS;
        // Each character row spans METATILE_H tile rows
        // Each character column spans METATILE_W tiles
        return (charRow * {c_name}_METATILE_H * {c_name}_TILES_PER_ROW) + (charCol * {c_name}_METATILE_W);
    }}
    return 0;  // Default to first tile (space)
}}

// Get tile index for a specific position within a character's metatile
// dx, dy are in 8x8 tile units (0 to METATILE_W-1, 0 to METATILE_H-1)
static inline uint16_t {c_name.lower()}_get_metatile_idx(char c, int dx, int dy) {{
    uint16_t base = {c_name.lower()}_get_tile(c);
    return base + (dy * {c_name}_TILES_PER_ROW) + dx;
}}

"""

    # Add lookup table for non-contiguous character sets
    if char_set != ''.join(chr(i) for i in range(ord(char_set[0]), ord(char_set[-1]) + 1)):
        header += f"// Character lookup table (for non-contiguous character sets)\n"
        header += f"static const uint8_t {c_name.lower()}_char_table[256] = {{\n"
        for i in range(0, 256, 16):
            values = [char_map.get(chr(j), 0) for j in range(i, min(i + 16, 256))]
            header += "    " + ", ".join(f"{v:3d}" for v in values) + ",\n"
        header += "};\n\n"

    header += f"#endif // {c_name}_CHARMAP_H\n"

    with open(path, 'w') as f:
        f.write(header)


def generate_grit_script(path, name, transparent_color):
    """Generate shell script for grit conversion"""

    script = f"""#!/bin/bash
# Grit conversion script for {name} font atlas
# Generated by ttf_to_atlas.py

GRIT=/opt/wonderful/thirdparty/blocksds/core/tools/grit/grit

# Convert font atlas to NDS format
# -gt: generate tiles
# -gB8: 8bpp tiles
# -mR!: NO tile reduction (critical for fonts! we need direct char->tile mapping)
# -mLf: flat map layout (linear tile indices)
# -p: generate palette
# -gT{transparent_color}: transparent color (magenta)
# -ftb: output as binary files

$GRIT {name}.png -gt -gB8 -mR! -mLf -p -gT{transparent_color} -ftb

# Rename outputs to standard naming
mv {name}.img.bin {name}_tiles.bin
# No map file needed - we calculate indices directly
rm -f {name}.map.bin 2>/dev/null
mv {name}.pal.bin {name}_pal.bin

echo "Converted {name} font atlas"
echo "Output files:"
echo "  {name}_tiles.bin - Tile graphics (linear, no reduction)"
echo "  {name}_pal.bin   - Palette"
"""

    with open(path, 'w') as f:
        f.write(script)
    os.chmod(path, 0o755)


def main():
    parser = argparse.ArgumentParser(
        description='Convert TTF font to NDS-compatible font atlas',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    %(prog)s myfont.ttf -o myfont
    %(prog)s myfont.ttf -o myfont --size 12 --metatile 8x16
    %(prog)s myfont.ttf -o myfont --chars "0123456789:/"
    %(prog)s myfont.ttf -o myfont --chars extended --columns 32
        """
    )

    parser.add_argument('ttf_file', help='Input TTF font file')
    parser.add_argument('-o', '--output', required=True,
                        help='Output base name (without extension)')
    parser.add_argument('--size', type=int, default=8,
                        help='Font size in pixels (default: 8)')
    parser.add_argument('--metatile', default='8x8',
                        help='Character cell size WxH (default: 8x8)')
    parser.add_argument('--chars', default='ascii',
                        help='Character set: ascii, extended, basic, or custom string')
    parser.add_argument('--columns', type=int, default=16,
                        help='Columns in atlas (default: 16)')
    parser.add_argument('--transparent', default='FF00FF',
                        help='Transparent color hex (default: FF00FF)')
    parser.add_argument('--antialias', action='store_true',
                        help='Enable antialiasing (not recommended)')

    args = parser.parse_args()

    # Validate inputs
    if not os.path.exists(args.ttf_file):
        print(f"Error: Font file not found: {args.ttf_file}")
        sys.exit(1)

    tile_w, tile_h = parse_metatile(args.metatile)

    # Validate tile dimensions are multiples of 8
    if tile_w % 8 != 0 or tile_h % 8 != 0:
        print("Warning: Metatile dimensions should be multiples of 8 for NDS")

    create_font_atlas(
        args.ttf_file,
        args.output,
        font_size=args.size,
        metatile=(tile_w, tile_h),
        chars=args.chars,
        columns=args.columns,
        transparent_color=args.transparent,
        antialias=args.antialias
    )


if __name__ == '__main__':
    main()
