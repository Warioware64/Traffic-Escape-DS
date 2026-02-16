#pragma once

#include <nds.h>
#include <filesystem.h>

#include <stdio.h>
#include <array>
#include <string>

namespace GridMesh
{

    inline void LoadGridMesh(void **ptr)
    {
        FILE* file;

        std::string pathGrid = "/Vehicules/Meshes/Grid_Board.bin";

        file = fopen(pathGrid.c_str(), "rb");
        fseek(file, 0, SEEK_END);

        size_t size_bytes_meshes = ftell(file);
        *ptr = malloc(size_bytes_meshes);
        rewind(file);

        fread(*ptr, sizeof(uint8_t), size_bytes_meshes, file);

        fclose(file);

        pathGrid.clear();
    };
}