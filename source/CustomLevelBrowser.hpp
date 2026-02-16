#pragma once

namespace CustomLevelBrowser
{
    // Result from the browser
    enum class Result {
        PLAY,       // User selected a level to play
        BACK        // User pressed BACK to return to menu
    };

    // Show the custom level browser
    // Returns PLAY if a level was selected (path stored internally)
    // Returns BACK if user wants to go back to menu
    Result Show();

    // Get the path of the selected level (valid after Show() returns PLAY)
    const char* GetSelectedPath();
}
