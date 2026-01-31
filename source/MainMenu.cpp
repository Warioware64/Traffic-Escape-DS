#include "MainMenu.hpp"

void MainMenu::TopBG()
{
    FILE* file;
    
    void *PtrImg;
    void *PtrMap;
    void *PtrPal;
    
    file = fopen("/BGs/topIntro.img.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/topIntro.map.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);
    
    file = fopen("/BGs/topIntro.pal.bin", "rb");

    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);
    int bg = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 2);  // Lower priority (higher number) than 3D layer 0
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE, PtrPal, size_bytes_pal);
    
    free(PtrImg);
    free(PtrMap);
    free(PtrPal);
}

void MainMenu::SubBG()
{
    FILE* file;
    
    void *PtrImg;
    void *PtrMap;
    void *PtrPal;
    
    file = fopen("/BGs/subIntro.img.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_img = ftell(file);
    PtrImg = malloc(size_bytes_img);
    rewind(file);

    fread(PtrImg, sizeof(uint8_t), size_bytes_img, file);

    fclose(file);

    file = fopen("/BGs/subIntro.map.bin", "rb");
    fseek(file, 0, SEEK_END);

    size_t size_bytes_map = ftell(file);
    PtrMap = malloc(size_bytes_map);
    rewind(file);

    fread(PtrMap, sizeof(uint8_t), size_bytes_map, file);

    fclose(file);
    
    file = fopen("/BGs/subIntro.pal.bin", "rb");

    fseek(file, 0, SEEK_END);

    size_t size_bytes_pal = ftell(file);
    PtrPal = malloc(size_bytes_pal);
    rewind(file);

    fread(PtrPal, sizeof(uint8_t), size_bytes_pal, file);

    fclose(file);
    int bg = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSetPriority(bg, 2);  // Lower priority (higher number) than 3D layer 0
    memcpy(bgGetGfxPtr(bg), PtrImg, size_bytes_img);
    memcpy(bgGetMapPtr(bg), PtrMap, size_bytes_map);
    memcpy(BG_PALETTE_SUB, PtrPal, size_bytes_pal);
    
    free(PtrImg);
    free(PtrMap);
    free(PtrPal);    
}
void MainMenu::Intro()
{
    if (!nitroFSInit(NULL))
        exit(1);
        
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    TopBG();
    SubBG();

    
    int brightness = 16;
    for (int i = 0; i < 120 ; i++)
    {
        if (brightness > 0)
        {
            brightness--;
            setBrightness(3, brightness);
        }
        swiWaitForVBlank();
    }

    while (brightness < 16)
    {
        swiWaitForVBlank();
        brightness++;
        setBrightness(3, brightness);
    }

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);


    
}