#pragma once

namespace OnlineLevelBrowser
{
    enum class Result {
        PLAY,
        BACK
    };

    Result Show();
    const char* GetSelectedPath();
}
