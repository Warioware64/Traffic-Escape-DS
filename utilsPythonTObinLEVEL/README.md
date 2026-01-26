# Traffic Escape DS - Level Editor

A Python tkinter-based visual level editor for creating Traffic Escape DS levels.

## Requirements

- Python 3.x
- tkinter (usually included with Python)

## Usage

```bash
python level_editor.py
```

## Features

- **Visual 6x6 grid editor** - Click to place and select cars
- **Car properties panel** - Set car type, orientation, texture, and position
- **Collision detection** - Prevents overlapping cars
- **Save/Load binary files** - Compatible with game's `LoadLevelFromFile()`
- **Export C++ header** - Generate hardcoded level data

## Binary File Format

Each level file contains:
- 1 byte: Number of cars (informational)
- 16 × 6 bytes: Car data (96 bytes total)

Each car entry (6 bytes):
| Offset | Size | Field |
|--------|------|-------|
| 0 | 1 | true_car (1 = active, 0 = empty slot) |
| 1 | 1 | carID (0-7) |
| 2 | 1 | orientation (0-3) |
| 3 | 1 | texture ID (0-23) |
| 4 | 1 | grid_x (0-5) |
| 5 | 1 | grid_y (0-5) |

## Car Types

| ID | Name | Size |
|----|------|------|
| 0 | Car | 2 cells |
| 1 | Car2 | 2 cells |
| 2 | Car3 | 2 cells |
| 3 | Car4 | 3 cells |
| 4 | Car5 | 2 cells |
| 5 | Car5Police | 2 cells |
| 6 | Car5Taxi | 2 cells |
| 7 | Car8 | 3 cells |

## Orientations

| ID | Description | Direction |
|----|-------------|-----------|
| 0 | Horizontal (left anchor) | LEFT_RIGHT |
| 1 | Horizontal (right anchor) | LEFT_RIGHT |
| 2 | Vertical (top anchor) | TOP_UP |
| 3 | Vertical (bottom anchor) | TOP_UP |

## Loading in Game

Place `.bin` files in `nitrofiles/Levels/` and load with:

```cpp
// Load from nitroFS
GameLevelLoader::LoadLevelFromFile("/Levels/level01.bin");
```

Or use the exported C++ header for hardcoded levels.
