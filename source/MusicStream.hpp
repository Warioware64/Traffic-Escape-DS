#pragma once

#include <nds.h>
#include <maxmod9.h>
#include <cstdio>
#include <cstring>

namespace MusicStream
{
    // Initialize maxmod and open WAV file for streaming
    // Must be called after nitroFSInit()
    bool Init(const char* wavPath);

    // Fill the circular buffer from the WAV file
    // Call this every frame from the main loop
    void Update();

    // Stop streaming and close the WAV file
    void Close();
}
