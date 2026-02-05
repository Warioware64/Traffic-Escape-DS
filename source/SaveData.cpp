#include "SaveData.hpp"
#include <fat.h>
#include <cstdio>
#include <cstring>

namespace SaveData
{

void Init()
{
    // Initialize with default values
    memcpy(saveData.magic, "TESD", 4);
    saveData.version = 1;
    memset(saveData.reserved, 0, sizeof(saveData.reserved));

    // Set all times to "not completed"
    for (int i = 0; i < MAX_LEVELS; i++) {
        saveData.bestTimes[i] = TIME_NOT_SET;
    }

    // Initialize FAT filesystem
    fatAvailable = fatInitDefault();
    if (!fatAvailable) {
        return;  // No FAT access, can't save
    }

    // Get the default drive (sd:/, fat:/, or nand:/) based on platform
    const char* defaultDrive = fatGetDefaultDrive();
    if (defaultDrive == NULL || defaultDrive[0] == '\0') {
        fatAvailable = false;
        return;  // No valid drive
    }
    snprintf(savePath, sizeof(savePath), "%sTrafficEscape.sav", defaultDrive);

    // Try to load existing save
    Load();
}

bool Load()
{
    if (!fatAvailable || savePath[0] == '\0') {
        return false;
    }

    FILE* file = fopen(savePath, "rb");
    if (!file) {
        return false;  // No save file exists yet
    }

    SaveFile tempData;
    size_t bytesRead = fread(&tempData, 1, sizeof(SaveFile), file);
    fclose(file);

    if (bytesRead != sizeof(SaveFile)) {
        return false;  // Corrupted or incomplete file
    }

    // Verify magic header
    if (memcmp(tempData.magic, "TESD", 4) != 0) {
        return false;  // Invalid save file
    }

    // Check version compatibility
    if (tempData.version > 1) {
        return false;  // Future version, can't read
    }

    // Copy loaded data
    memcpy(&saveData, &tempData, sizeof(SaveFile));
    return true;
}

bool Save()
{
    if (!fatAvailable || savePath[0] == '\0') {
        return false;
    }

    FILE* file = fopen(savePath, "wb");
    if (!file) {
        return false;  // Can't create/open file
    }

    size_t bytesWritten = fwrite(&saveData, 1, sizeof(SaveFile), file);
    fclose(file);

    return (bytesWritten == sizeof(SaveFile));
}

uint32_t GetBestTime(int level)
{
    if (level < 0 || level >= MAX_LEVELS) {
        return TIME_NOT_SET;
    }
    return saveData.bestTimes[level];
}

bool SetBestTime(int level, uint32_t timeFrames)
{
    if (level < 0 || level >= MAX_LEVELS) {
        return false;
    }

    uint32_t currentBest = saveData.bestTimes[level];

    // Check if this is a new record (lower time is better)
    if (currentBest == TIME_NOT_SET || timeFrames < currentBest) {
        saveData.bestTimes[level] = timeFrames;
        Save();  // Persist to disk
        return true;  // New record!
    }

    return false;  // Not a new record
}

bool IsLevelCompleted(int level)
{
    return GetBestTime(level) != TIME_NOT_SET;
}

void FormatTime(uint32_t frames, char* buffer, size_t bufferSize)
{
    if (frames == TIME_NOT_SET) {
        snprintf(buffer, bufferSize, "--:--.---");
        return;
    }

    // Convert frames to time components (60fps)
    uint32_t totalSeconds = frames / 60;
    uint32_t remainingFrames = frames % 60;
    uint32_t milliseconds = (remainingFrames * 1000) / 60;
    uint32_t minutes = totalSeconds / 60;
    uint32_t seconds = totalSeconds % 60;

    snprintf(buffer, bufferSize, "%02lu:%02lu.%03lu",
             (unsigned long)minutes, (unsigned long)seconds, (unsigned long)milliseconds);
}

} // namespace SaveData
