#include <nds.h>
#include <stdio.h>
#include <filesystem.h>
#include "etl/profiles/armv5.h"
#include "etl/vector.h"
#include "InitEngineMode.hpp"
//#include "car_combined_bin.h"
#include "Vehicules/Meshes/Car_bin.h"
#include "Vehicules/Textures/car_pal_bin.h"
#include "Vehicules/Textures/car_tex_bin.h"
//#include "teapot_bin.h"


__attribute__((noreturn)) void wait_forever(void)
{
    printf("Press START to exit.");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            exit(1);
    }
}

bool file_load(const char *path, void **buffer, size_t *size)
{
    // Open the file in read binary mode
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        perror("fopen");
        return false;
    }

    // Move read cursor to the end of the file
    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return false;
    }

    // Check position of the cursor (we're at the end, so this is the size)
    size_t size_ = ftell(f);
    if (size_ == 0)
    {
        printf("Size is 0!");
        fclose(f);
        return false;
    }

    // Move cursor to the start of the file again
    rewind(f);

    // Allocate buffer to hold data
    *buffer = malloc(size_);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        return false;
    }

    // Read all data into the buffer
    if (fread(*buffer, size_, 1, f) != 1)
    {
        perror("fread");
        fclose(f);
        free(*buffer);
        return false;
    }

    // Close file
    ret = fclose(f);
    if (ret != 0)
    {
        perror("fclose");
        free(*buffer);
        return false;
    }

    if (size)
        *size = size_;

    return true;
}


int main(int argc, char **argv)
{
    consoleDemoInit();
    lcdMainOnBottom();
    int textureID;

    nitroFSInit(NULL);

    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);

    glClearColor(0, 0, 0, 0);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

        // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    void *car_texture = malloc(16384);
    decompress(car_tex_bin, (uint8_t*)car_texture, LZ77);

    if (glTexImage2D(0, 0, GL_RGB256, 128, 128, 0, TEXGEN_TEXCOORD, car_texture) == 0)
    {
        printf("Failed to load texture\n");
        wait_forever();
    }

    void *car_palette = malloc(512);
    
    decompress(car_pal_bin, (uint8_t*)car_palette, LZ77);

    if (glColorTableEXT(0, 0, 256, 0, 0, (const uint16_t *)car_palette) == 0)
    {
        printf("Failed to load palette\n");
        wait_forever();
    }
    //glDisable(GL_TEXTURE_2D);
    /*
    etl::vector<uint8_t, 64> data;

    data.push_back(42);
    data.push_back(15);
    data.push_back(12);
    data.push_back(78);
    data.push_back(35);
    data.push_back(42);*/
    
    void *car = malloc(15136);
    decompress(Car_bin, (uint8_t*)car, LZ77);

    /*void *car;
    if (!file_load("Vehicules/Meshes/Car.bin", &car, NULL))
    {
        printf("Failed to load file\n");
        wait_forever();
    }*/

    /*
    typedef struct {
        int width, height;
        int id;
    } texture_info_t;*/

#define NUM_TEXTURES 1

    //texture_info_t tex[NUM_TEXTURES] = { 0 };

    //tex[0].id = load_texture_grf("Vehicules/Textures/car.grf", &tex[0].width, &tex[0].height);
    /*
    for (const auto value : data)
    {
        printf("Value: %u\n", value);
    }*/
    //data.insert((const unsigned char*)(3), 99);
    printf("Hello, test world!\n");
    /*data.insert((data.begin() + 3), 99);
    for (const auto value : data)
    {
        printf("Value: %u\n", value);
    }*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
            0.0, 0.0, 0.0,  // Look at
            0.0, 1.0, 0.0); // Up
    
    printf("Hello, test world!\n");
    glMatrixMode(GL_MODELVIEW);

    // Setup some material properties
    glMaterialf(GL_DIFFUSE, RGB15(20, 20, 20));
    glMaterialf(GL_AMBIENT, RGB15(16, 16, 16));
    
    glMaterialf(GL_SPECULAR, RGB15(8, 8, 8));
    glMaterialf(GL_EMISSION, RGB15(5, 5, 5));

    // Setup lights
    glLight(0, RGB15(31, 31, 31), floattov10(-1), floattov10(0), floattov10(0));
    glLight(1, RGB15(31, 31, 31), floattov10(0), floattov10(0), floattov10(-1));
    while (1)
    {
        swiWaitForVBlank();


        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //gluLookAt(0,0,4, 0,0,0, 0,1,0);
        gluLookAt(0.0, 3.0, 4.0,  // Position
                  0.0, 0.0, 0.0,  // Look at
                  0.0, 1.0, 0.0); // Up
        glPushMatrix();
        glPolyFmt(
            POLY_ALPHA(31) |
            POLY_CULL_BACK |
            POLY_FORMAT_LIGHT0 |
            POLY_FORMAT_LIGHT1
        );
        glColor3b(0,0,0);
        glCallList(car);
        //glPopMatrix(1);
        glFlush(GL_ZBUFFERING);
    }

    return 0;
}
