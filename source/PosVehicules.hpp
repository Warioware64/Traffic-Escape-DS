#pragma once

#include <nds.h>
#include <filesystem.h>

#include <algorithm>
#include <stdio.h>
#include <array>
#include <string>

enum class OrientationRULES : uint8_t
{
    LEFT_RIGHT,
    TOP_UP
};



struct Grid2D
{
    uint8_t x, y;

    bool operator==(const Grid2D& other) const
    {
        return x == other.x && y == other.y;
    }
};


struct BasePos
{
    float x, y, z;
};

struct CarsStates
{
    uint8_t true_car;
    void *ptrMesh;
    int texGLptr;
    size_t carID;
    size_t orientation;
    size_t tex;
    BasePos basepose;
    Grid2D grid2d;
};

using LevelData = std::array<CarsStates, 16>;

namespace PosVehicules
{
    constexpr inline std::array<BasePos, 4> BasePoses = {{
        {-1.25, 2.25, 0.5},
        {-0.75, 2.25, 0.5},
        {-1.25, 1.75, 0.5},
        {-1.25, 2.25, 0.5}
    }};
    
    constexpr inline std::array<OrientationRULES, 4> OrientationRULESpreset = {OrientationRULES::LEFT_RIGHT,
                                                                             OrientationRULES::LEFT_RIGHT,
                                                                            OrientationRULES::TOP_UP,
                                                                            OrientationRULES::TOP_UP};
    
    inline std::array<std::string, 8> CarNames = {"Car", "Car2", "Car3", "Car4", "Car5", "Car5Police", "Car5Taxi", "Car8"};
    inline std::array<std::string, 4> OrientationNames = {"_o1.bin", "_o2.bin", "_o3.bin", "_o4.bin"};
    inline std::array<std::string, 24> TextureNames = {"car", "car_blue", "car_gray", "car_red"
                                                        , "car2", "car2_black", "car2_red", "car3", "car3_red"
                                                        , "car3_yellow", "car4", "car4_grey", "car4_lightgrey"
                                                        , "car4_lightorange", "car5", "car5_green", "car5_grey"
                                                        , "car5_police", "car5_police_la", "car5_taxi"
                                                        , "Car8", "Car8_grey", "Car8_mail", "Car8_purple"};

    

    inline void LoadVehicule_Mesh(void *&ptr, size_t carID, size_t orientation)
    {
        FILE* file;
        
        //consoleDebugInit(DebugDevice_NOCASH);
        std::string path = "/Vehicules/Meshes/" + PosVehicules::CarNames.at(carID) + PosVehicules::OrientationNames.at(orientation);
        //printf("Here is result: %s", path.c_str());
        file = fopen(path.c_str(), "rb");
        fseek(file, 0, SEEK_END);

        size_t size_bytes_meshes = ftell(file);
        ptr = malloc(size_bytes_meshes);
        rewind(file);

        fread(ptr, sizeof(uint8_t), size_bytes_meshes, file);

        fclose(file);

        path.clear();

    };
    
    inline void LoadVehicule_Texture(int *texGLptr, size_t textureID)
    {
        FILE* file;

        void *TexPtr;
        void *PalPtr;

        std::string pathTex = "/Vehicules/Textures/" + PosVehicules::TextureNames.at(textureID) + "_tex.bin";

        file = fopen(pathTex.c_str(), "rb");
        fseek(file, 0, SEEK_END);

        size_t size_bytes_tex = ftell(file);

        TexPtr = malloc(size_bytes_tex);

        rewind(file);

        fread(TexPtr, sizeof(uint8_t), size_bytes_tex, file);

        fclose(file);

        pathTex.clear();

        std::string pathPal = "/Vehicules/Textures/" + PosVehicules::TextureNames.at(textureID) + "_pal.bin";
        file = fopen(pathPal.c_str(), "rb");
        fseek(file, 0, SEEK_END);

        size_t size_bytes_pal = ftell(file);

        PalPtr = malloc(size_bytes_pal);

        rewind(file);

        fread(PalPtr, sizeof(uint8_t), size_bytes_pal, file);

        fclose(file);

        pathPal.clear();

        glGenTextures(1, texGLptr);
        
        glBindTexture(0, *texGLptr);

        glTexImage2D(0, 0, GL_RGB256, 128, 128, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                     (const int32_t*)TexPtr);

        glColorTableEXT(0, 0, 256, 0, 0, (const int32_t*)PalPtr);

        free(PalPtr);
        free(TexPtr);


    };
}

