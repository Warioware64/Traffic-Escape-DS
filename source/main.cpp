


#include <nds.h>
#include <stdio.h>
#include <filesystem.h>
#include "etl/profiles/armv5.h"
#include "etl/vector.h"
#include <NEMain.h>
#include "CarsAssets128128.h"

typedef struct {
    int placeholder;
    NE_Material *material;
    NE_Palette *palette;
    NE_Sprite *sprite;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    //(void)Scene; // Silence unused variable warning

    NE_2DViewInit();
    NE_SpriteDrawAll();
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_InitConsole();

    Scene.material = NE_MaterialCreate();
    Scene.palette = NE_PaletteCreate();
    nitroFSInit(NULL);
    NE_MaterialTexLoad(Scene.material, NE_PAL256, 128, 128,
                          NE_TEXTURE_COLOR0_TRANSPARENT,
                          CarsAssets128128Bitmap);

    NE_PaletteLoad(Scene.palette, CarsAssets128128Pal, 256, NE_PAL256);
    NE_MaterialSetPalette(Scene.material, Scene.palette);

    //NE_MaterialTexLoadGRF(Scene.material, Scene.palette, NE_TEXGEN_TEXCOORD,"nitro:/CarsAssets128128.grf");

    Scene.sprite = NE_SpriteCreate();
    
    NE_SpriteSetMaterial(Scene.sprite, Scene.material);
    NE_SpriteSetMaterialCanvas(Scene.sprite, 0, 3, 20, 43);
    NE_SpriteSetPos(Scene.sprite, 100, 80);
    NE_SpriteSetSize(Scene.sprite, 20, 40);


    while (1)
    {
        NE_WaitForVBL(NE_UPDATE_ANIMATIONS);
        NE_ProcessArg(Draw3DScene, &Scene);

        scanKeys();

        // Your code goes here
    }

    return 0;
}