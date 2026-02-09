#pragma once

#include <nds.h>
#include <cstdint>

namespace SaveData
{
    // Maximum number of levels to track
    constexpr int MAX_LEVELS = 20;

    // Time value indicating level not completed
    constexpr uint32_t TIME_NOT_SET = 0xFFFFFFFF;

    // Save data structure
    struct SaveFile {
        char magic[4];           // "TESD" magic identifier
        uint8_t version;         // Save format version
        uint8_t reserved[3];     // Padding
        uint32_t bestTimes[MAX_LEVELS];  // Best time in frames for each level (0xFFFFFFFF = not set)
    };

    // Current save data in memory
    inline SaveFile saveData;

    // Save file path
    inline char savePath[64] = {0};

    // Whether FAT is available for saving
    inline bool fatAvailable = false;

    // Dirty flag - save data changed in memory but not yet written to disk
    inline bool dirty = false;

    // Initialize save system (calls fatInitDefault)
    void Init();

    // Load save file from disk
    bool Load();

    // Save current data to disk
    bool Save();

    // Flush dirty data to disk (call at safe transition points)
    void Flush();

    // Get best time for a level (0-based index)
    // Returns TIME_NOT_SET if level not completed
    uint32_t GetBestTime(int level);

    // Set best time for a level if it's better than existing
    // Returns true if this was a new record
    bool SetBestTime(int level, uint32_t timeFrames);

    // Check if a level has been completed
    bool IsLevelCompleted(int level);

    // Format time (in frames) to string "MM:SS.mmm"
    void FormatTime(uint32_t frames, char* buffer, size_t bufferSize);
}
