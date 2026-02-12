#include "MusicStream.hpp"
#include "soundbank.h"

#include <filesystem.h>

#define RIFF_ID 0x46464952
#define WAVE_ID 0x45564157
#define FMT_ID  0x20746d66
#define DATA_ID 0x61746164

namespace MusicStream
{

struct WAVHeader
{
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
    uint32_t subchunk1ID;
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t subchunk2ID;
    uint32_t subchunk2Size;
};

static constexpr int BUFFER_LENGTH = 16384;

static FILE* wavFile = nullptr;
static char streamBuffer[BUFFER_LENGTH];
static int bufferIn = 0;
static int bufferOut = 0;
static bool initialized = false;

// Streaming callback - runs in interrupt context, reads from circular buffer
static mm_word streamingCallback(mm_word length, mm_addr dest, mm_stream_formats format)
{
    size_t multiplier = 0;

    if (format == MM_STREAM_8BIT_MONO)
        multiplier = 1;
    else if (format == MM_STREAM_8BIT_STEREO)
        multiplier = 2;
    else if (format == MM_STREAM_16BIT_MONO)
        multiplier = 2;
    else if (format == MM_STREAM_16BIT_STEREO)
        multiplier = 4;

    size_t size = length * multiplier;
    size_t bytesUntilEnd = BUFFER_LENGTH - bufferOut;

    if (bytesUntilEnd > size)
    {
        memcpy(dest, &streamBuffer[bufferOut], size);
        bufferOut += size;
    }
    else
    {
        char* dst = (char*)dest;
        memcpy(dst, &streamBuffer[bufferOut], bytesUntilEnd);
        dst += bytesUntilEnd;
        size -= bytesUntilEnd;
        memcpy(dst, &streamBuffer[0], size);
        bufferOut = size;
    }

    return length;
}

// Read bytes from WAV file, looping back to data start on EOF
static void readFile(char* buffer, size_t size)
{
    while (size > 0)
    {
        int res = fread(buffer, 1, size, wavFile);
        size -= res;
        buffer += res;

        if (feof(wavFile))
        {
            // Loop back to audio data start (skip header)
            fseek(wavFile, sizeof(WAVHeader), SEEK_SET);
            res = fread(buffer, 1, size, wavFile);
            size -= res;
            buffer += res;
        }
    }
}

// Fill circular buffer from file
static void fillBuffer(bool forceFill)
{
    if (!forceFill)
    {
        if (bufferIn == bufferOut)
            return;
    }

    if (bufferIn < bufferOut)
    {
        size_t size = bufferOut - bufferIn;
        readFile(&streamBuffer[bufferIn], size);
        bufferIn += size;
    }
    else
    {
        size_t size = BUFFER_LENGTH - bufferIn;
        readFile(&streamBuffer[bufferIn], size);
        bufferIn = 0;

        size = bufferOut - bufferIn;
        readFile(&streamBuffer[bufferIn], size);
        bufferIn += size;
    }

    if (bufferIn >= BUFFER_LENGTH)
        bufferIn -= BUFFER_LENGTH;
}

static mm_stream_formats getMMStreamType(uint16_t numChannels, uint16_t bitsPerSample)
{
    if (numChannels == 1)
    {
        if (bitsPerSample == 8)
            return MM_STREAM_8BIT_MONO;
        else
            return MM_STREAM_16BIT_MONO;
    }
    else
    {
        if (bitsPerSample == 8)
            return MM_STREAM_8BIT_STEREO;
        else
            return MM_STREAM_16BIT_STEREO;
    }
}

bool Init(const char* wavPath)
{
    // Initialize maxmod with soundbank from nitroFS (for SFX)
    mmInitDefault("nitro:/soundbank.bin");

    // Load sound effects
    mmLoadEffect(SFX_SCI_FI_DESELECT);
    mmLoadEffect(SFX_XYLOPHONE_LEVEL_COMPLETE);

    // Open WAV file for streaming music
    wavFile = fopen(wavPath, "rb");
    if (wavFile == nullptr)
        return false;

    WAVHeader header = {};
    if (fread(&header, 1, sizeof(WAVHeader), wavFile) != sizeof(WAVHeader))
    {
        fclose(wavFile);
        wavFile = nullptr;
        return false;
    }

    // Validate WAV header
    if (header.chunkID != RIFF_ID || header.format != WAVE_ID ||
        header.subchunk1ID != FMT_ID || header.subchunk2ID != DATA_ID)
    {
        fclose(wavFile);
        wavFile = nullptr;
        return false;
    }

    // Pre-fill the circular buffer before starting playback
    fillBuffer(true);

    // Open the stream
    mm_stream stream = {};
    stream.sampling_rate = header.sampleRate;
    stream.buffer_length = 2048;
    stream.callback = streamingCallback;
    stream.format = getMMStreamType(header.numChannels, header.bitsPerSample);
    stream.timer = MM_TIMER0;
    stream.manual = false;
    mmStreamOpen(&stream);

    initialized = true;
    return true;
}

void Update()
{
    if (!initialized || wavFile == nullptr)
        return;

    fillBuffer(false);
}

void PlaySFX(mm_word sfxID)
{
    mmEffect(sfxID);
}

void Close()
{
    if (!initialized)
        return;

    mmStreamClose();

    if (wavFile != nullptr)
    {
        fclose(wavFile);
        wavFile = nullptr;
    }

    initialized = false;
}

} // namespace MusicStream
