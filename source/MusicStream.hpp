#pragma once

#include <nds.h>
#include <maxmod9.h>
#include <cstdio>
#include <cstring>

namespace MusicStream
{
    // Initialize maxmod (soundbank + streaming) and open WAV file
    // Must be called after nitroFSInit()
    bool Init(const char* wavPath);

    // Fill the circular buffer from the WAV file
    // Call this every frame from the main loop
    void Update();

    // Play a sound effect by its soundbank ID
    void PlaySFX(mm_word sfxID);

    // Stop streaming and close the WAV file
    void Close();
}
