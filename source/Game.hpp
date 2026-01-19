#pragma once
#include <nds.h>
#include <filesystem.h>

namespace Game
{
    inline int textureID;
    inline float x_test;
    inline float y_test;
    inline float z_test;

    inline size_t idMesh;
    inline size_t idOrient;
    inline size_t idTex;
    
    inline void *car;
    inline void *grid;

    void Init();
    void Update();
}