#include "OnlineLevelBrowser.hpp"
#include "GameLevelLoader.hpp"
#include "GridMesh.hpp"
#include "BGFont.hpp"
#include "MusicStream.hpp"
#include "soundbank.h"
#include "cJSON.h"

#include <nds.h>
#include <dswifi9.h>
#include <curl/curl.h>

#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

// Base URL for community levels
static const char* BASE_URL =
    "https://raw.githubusercontent.com/Warioware64/"
    "Community-Levels-Traffic-Escape-DS/refs/heads/main/CommunityLevels/";

constexpr int MAX_ONLINE_LEVELS = 64;

// Temp file for downloaded level JSON
static const char* TEMP_PATH_SD  = "sd:/TE_LEVELS/_online_temp.json";
static const char* TEMP_PATH_FAT = "fat:/TE_LEVELS/_online_temp.json";

// Level metadata
static std::vector<std::string> levelFilenames;
static std::vector<std::string> levelNames;
static std::vector<std::string> levelAuthors;
static int currentIndex = 0;
static char selectedPath[256] = {0};

// Current level's raw JSON (kept for writing to temp file on PLAY)
static std::string currentLevelJSON;

// 3D preview state
static void* previewGrid = nullptr;

// WiFi/curl state (persist across Show() calls - init once, never fully deinit)
static bool wifiInitialized = false;   // Wifi_InitDefault called once
static bool wifiConnected = false;     // Currently associated with AP
static const char* errorMessage = nullptr;

// ---------------------------------------------------------------------------
// libcurl write callback
// ---------------------------------------------------------------------------
static size_t CurlWriteCallback(char* data, size_t size, size_t nmemb, std::string* outBuffer)
{
    if (!outBuffer)
        return 0;
    outBuffer->append(data, size * nmemb);
    return size * nmemb;
}

// ---------------------------------------------------------------------------
// HTTP GET helper
// ---------------------------------------------------------------------------
static bool FetchURL(const char* url, std::string& outBuffer)
{
    outBuffer.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        errorMessage = "curl_easy_init failed";
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_3);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        errorMessage = curl_easy_strerror(res);
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// WiFi init / cleanup
//
// WiFi + curl are initialized once and kept alive across Show() calls.
// DisableWifi/Deinit can't be called while lwIP is active (used by curl),
// so we only disconnect from the AP when leaving the browser.
// ---------------------------------------------------------------------------
static bool InitWiFi()
{
    // One-time WiFi + curl initialization
    if (!wifiInitialized)
    {
        if (!Wifi_InitDefault(INIT_ONLY | WIFI_ATTEMPT_DSI_MODE))
        {
            errorMessage = "WiFi init failed";
            return false;
        }

        CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
        if (res != CURLE_OK)
        {
            errorMessage = "curl_global_init failed";
            return false;
        }

        wifiInitialized = true;
    }

    // Start async connection using WFC firmware settings
    Wifi_AutoConnect();

    // Poll until connected (non-blocking loop keeps music alive)
    while (true)
    {
        int status = Wifi_AssocStatus();
        if (status == ASSOCSTATUS_ASSOCIATED)
            break;
        if (status == ASSOCSTATUS_CANNOTCONNECT)
        {
            errorMessage = "WiFi connection failed";
            return false;
        }

        MusicStream::Update();
        swiWaitForVBlank();
        scanKeys();
    }

    wifiConnected = true;
    return true;
}

static void CleanupWiFi()
{
    // Just disconnect from AP - don't disable/deinit WiFi or curl
    // (lwIP must stay active, and re-init would assert)
    if (wifiConnected)
    {
        Wifi_DisconnectAP();
        wifiConnected = false;
    }
}

// ---------------------------------------------------------------------------
// Fetch index.json and populate filename list
// ---------------------------------------------------------------------------
static bool FetchIndex()
{
    std::string url = std::string(BASE_URL) + "index.json";
    std::string body;

    if (!FetchURL(url.c_str(), body))
        return false;

    cJSON* j = cJSON_Parse(body.c_str());
    if (!j || !cJSON_IsArray(j))
    {
        if (j) cJSON_Delete(j);
        errorMessage = "Invalid index.json";
        return false;
    }

    levelFilenames.clear();
    levelNames.clear();
    levelAuthors.clear();

    int count = cJSON_GetArraySize(j);
    for (int i = 0; i < count && i < MAX_ONLINE_LEVELS; i++)
    {
        cJSON* item = cJSON_GetArrayItem(j, i);
        if (item && cJSON_IsString(item))
        {
            levelFilenames.push_back(item->valuestring);
            levelNames.push_back(item->valuestring);  // filename as placeholder
            levelAuthors.push_back("...");
        }
    }

    cJSON_Delete(j);
    return !levelFilenames.empty();
}

// ---------------------------------------------------------------------------
// Write current JSON to temp file, returns path or nullptr on failure
// ---------------------------------------------------------------------------
static const char* WriteTempFile()
{
    const char* path = TEMP_PATH_SD;
    FILE* f = fopen(path, "wb");
    if (!f)
    {
        path = TEMP_PATH_FAT;
        f = fopen(path, "wb");
    }
    if (!f)
    {
        errorMessage = "Can't write temp file";
        return nullptr;
    }
    fwrite(currentLevelJSON.c_str(), 1, currentLevelJSON.size(), f);
    fclose(f);
    return path;
}

// ---------------------------------------------------------------------------
// Fetch a level JSON, parse metadata, write temp file for preview
// ---------------------------------------------------------------------------
static bool FetchAndLoadLevel(int index)
{
    if (index < 0 || index >= (int)levelFilenames.size())
        return false;

    std::string url = std::string(BASE_URL) + levelFilenames[index];

    if (!FetchURL(url.c_str(), currentLevelJSON))
        return false;

    // Parse metadata
    cJSON* j = cJSON_Parse(currentLevelJSON.c_str());
    if (!j)
    {
        errorMessage = "Invalid level JSON";
        return false;
    }

    cJSON* nameItem = cJSON_GetObjectItem(j, "level_name");
    if (nameItem && cJSON_IsString(nameItem))
        levelNames[index] = nameItem->valuestring;

    cJSON* authorItem = cJSON_GetObjectItem(j, "author");
    if (authorItem && cJSON_IsString(authorItem))
        levelAuthors[index] = authorItem->valuestring;

    cJSON_Delete(j);

    // Write to temp file so LoadLevelFromJSON can read it
    const char* path = WriteTempFile();
    if (!path)
        return false;

    strncpy(selectedPath, path, sizeof(selectedPath) - 1);
    selectedPath[sizeof(selectedPath) - 1] = '\0';

    return true;
}

// ---------------------------------------------------------------------------
// 3D preview (same as CustomLevelBrowser)
// ---------------------------------------------------------------------------
static void CleanupPreview()
{
    for (auto& car : GameLevelLoader::lev_data)
    {
        if (car.ptrMesh != nullptr)
        {
            free(car.ptrMesh);
            car.ptrMesh = nullptr;
        }
        car.true_car = 0;
    }
    PosVehicules::ClearTextureCache();

    if (previewGrid != nullptr)
    {
        free(previewGrid);
        previewGrid = nullptr;
    }
}

static void LoadPreview()
{
    CleanupPreview();

    if (currentIndex < 0 || currentIndex >= (int)levelFilenames.size())
        return;

    GridMesh::LoadGridMesh(&previewGrid);
    GameLevelLoader::LoadLevelFromJSON(selectedPath);

    // Sky BG now in VRAM — make 3D clear transparent so it shows through
    glClearColor(0, 0, 0, 0);
}

static void RenderPreview()
{
    glPushMatrix();
    glTranslatef(-1.25, -0.25, 0.5);
    glRotateX(85);
    glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
    glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT1);
    glBindTexture(0, 0);
    if (previewGrid)
        glCallList(previewGrid);
    glPopMatrix(1);

    for (const auto& car : GameLevelLoader::lev_data)
    {
        if (car.true_car != 0)
        {
            glPushMatrix();
            glTranslatef(car.basepose.x + (car.grid2d.x * 0.5f),
                        car.basepose.y + (car.grid2d.y * -0.5f),
                        car.basepose.z);
            glRotateX(85);
            glScalef32(floattof32(0.5), floattof32(0.5), floattof32(0.5));
            glPolyFmt(POLY_ALPHA(31) | POLY_ID(0) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);
            glBindTexture(0, car.texGLptr);
            glCallList(car.ptrMesh);
            glPopMatrix(1);
        }
    }

    glFlush(0);
}

// ---------------------------------------------------------------------------
// Browser UI
// ---------------------------------------------------------------------------
static void DrawStatusScreen(const char* line1, const char* line2 = nullptr)
{
    BGFont::Clear();
    BGFont::Print(5, 0, "ONLINE LEVELS");
    BGFont::Print(4, 5, line1);
    if (line2)
        BGFont::Print(2, 7, line2);
}

static void DrawBrowserUI()
{
    BGFont::Clear();

    if (levelFilenames.empty())
    {
        BGFont::Print(3, 4, "No online levels found!");
        BGFont::Print(10, 11, "[ BACK ]");
        return;
    }

    BGFont::Print(5, 0, "ONLINE LEVELS");
    BGFont::Printf(8, 1, "%d / %d", currentIndex + 1, (int)levelFilenames.size());

    if (currentIndex < (int)levelNames.size())
    {
        const std::string& name = levelNames[currentIndex];
        if (name.length() > 28)
            BGFont::Printf(1, 3, "%.28s", name.c_str());
        else
            BGFont::Print(1, 3, name.c_str());
    }

    if (currentIndex < (int)levelAuthors.size())
        BGFont::Printf(1, 5, "by %s", levelAuthors[currentIndex].c_str());

    BGFont::Print(2, 9, "[ PREV ]");
    BGFont::Print(12, 9, "[ PLAY ]");
    BGFont::Print(22, 9, "[ NEXT ]");
    BGFont::Print(10, 11, "[ BACK ]");
}

static void DrawErrorUI()
{
    BGFont::Clear();
    BGFont::Print(5, 0, "ONLINE LEVELS");
    BGFont::Print(6, 4, "ERROR:");
    if (errorMessage)
    {
        char truncated[30];
        strncpy(truncated, errorMessage, 29);
        truncated[29] = '\0';
        BGFont::Print(1, 6, truncated);
    }
    BGFont::Print(10, 11, "[ BACK ]");
}

// ---------------------------------------------------------------------------
// Touch input (same layout as CustomLevelBrowser)
// ---------------------------------------------------------------------------
enum class BrowserAction {
    NONE, PREV, NEXT, PLAY, BACK
};

static BrowserAction HandleBrowserTouch()
{
    scanKeys();

    if (keysDown() & KEY_TOUCH)
    {
        touchPosition touch;
        touchRead(&touch);

        int charX = touch.px / 8;
        int charY = touch.py / 16;

        if (charY == 9 && charX >= 2 && charX < 10)
            return BrowserAction::PREV;
        if (charY == 9 && charX >= 12 && charX < 20)
            return BrowserAction::PLAY;
        if (charY == 9 && charX >= 22 && charX < 30)
            return BrowserAction::NEXT;
        if (charY == 11 && charX >= 10 && charX < 18)
            return BrowserAction::BACK;
    }

    return BrowserAction::NONE;
}

// ---------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------
static void CleanupBrowser()
{
    levelFilenames.clear();
    levelFilenames.shrink_to_fit();
    levelNames.clear();
    levelNames.shrink_to_fit();
    levelAuthors.clear();
    levelAuthors.shrink_to_fit();
    currentLevelJSON.clear();
    currentLevelJSON.shrink_to_fit();
}

// ---------------------------------------------------------------------------
// Setup 3D engine on main screen (deferred until a preview is ready)
// ---------------------------------------------------------------------------
static bool glInitialized = false;

static void Setup3DEngine()
{
    setBrightness(3, 16);

    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankE(VRAM_E_MAIN_BG);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    glInit();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    glClearColor(0, 0, 0, 31);
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0,
              0.0, 1.0, 0.0,
              0.0, 1.0, 0.0);

    glMaterialf(GL_AMBIENT, RGB15(0, 0, 0));
    glMaterialf(GL_DIFFUSE, RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glLight(0, RGB15(31, 31, 31), floattov10(-1.0), floattov10(-1.0), floattov10(-1.0));
    glLight(1, RGB15(31, 31, 31), floattov10(-0.75), floattov10(-0.75), floattov10(-0.75));

    lcdMainOnTop();
    setBrightness(3, 0);

    glInitialized = true;
}

// ---------------------------------------------------------------------------
// Main entry point
// ---------------------------------------------------------------------------
OnlineLevelBrowser::Result OnlineLevelBrowser::Show()
{
    // Keep previous screen content visible until a preview is ready
    // Sub screen (VRAM_C/H) not touched - persists from MainMenu

    errorMessage = nullptr;
    currentIndex = 0;
    bool hasLevels = false;
    bool inError = false;
    glInitialized = false;

    // Mute hardware sound during WiFi connect + fetch (avoids soft-lock audio)
    soundSetMasterVolume(0);

    // Phase 1: Connect WiFi
    DrawStatusScreen("Connecting to WiFi...");
    swiWaitForVBlank();
    bgUpdate();

    if (!InitWiFi())
    {
        inError = true;
    }
    else
    {
        // Phase 2: Fetch index
        DrawStatusScreen("Fetching level list...");
        swiWaitForVBlank();
        bgUpdate();

        if (!FetchIndex())
        {
            inError = true;
        }
        else
        {
            // Phase 3: Fetch first level
            DrawStatusScreen("Loading level 1...");
            swiWaitForVBlank();
            bgUpdate();

            if (FetchAndLoadLevel(0))
            {
                // Now setup 3D engine and show preview
                Setup3DEngine();
                LoadPreview();
                hasLevels = true;
            }
            else
            {
                inError = true;
            }
        }
    }

    // Restore sound volume
    soundSetMasterVolume(127);

    if (inError)
        DrawErrorUI();
    else
        DrawBrowserUI();

    // Phase 4: Browse loop
    while (true)
    {
        swiWaitForVBlank();
        bgUpdate();
        MusicStream::Update();

        if (hasLevels && !inError)
            RenderPreview();

        BrowserAction action = HandleBrowserTouch();

        // In error state, only BACK works
        if (inError)
        {
            if (action == BrowserAction::BACK)
            {
                MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                CleanupPreview();
                if (glInitialized) glDeinit();
                CleanupWiFi();
                CleanupBrowser();
                return Result::BACK;
            }
            continue;
        }

        switch (action)
        {
            case BrowserAction::PREV:
                if (hasLevels)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    currentIndex--;
                    if (currentIndex < 0)
                        currentIndex = (int)levelFilenames.size() - 1;

                    DrawStatusScreen("Loading level...");
                    swiWaitForVBlank();
                    bgUpdate();

                    soundSetMasterVolume(0);
                    if (FetchAndLoadLevel(currentIndex))
                        LoadPreview();
                    soundSetMasterVolume(127);

                    DrawBrowserUI();
                }
                break;

            case BrowserAction::NEXT:
                if (hasLevels)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    currentIndex++;
                    if (currentIndex >= (int)levelFilenames.size())
                        currentIndex = 0;

                    DrawStatusScreen("Loading level...");
                    swiWaitForVBlank();
                    bgUpdate();

                    soundSetMasterVolume(0);
                    if (FetchAndLoadLevel(currentIndex))
                        LoadPreview();
                    soundSetMasterVolume(127);

                    DrawBrowserUI();
                }
                break;

            case BrowserAction::PLAY:
                if (hasLevels)
                {
                    MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                    // selectedPath already set by FetchAndLoadLevel
                    CleanupPreview();
                    glDeinit();
                    CleanupWiFi();
                    CleanupBrowser();
                    return Result::PLAY;
                }
                break;

            case BrowserAction::BACK:
                MusicStream::PlaySFX(SFX_SCI_FI_DESELECT);
                CleanupPreview();
                glDeinit();
                CleanupWiFi();
                CleanupBrowser();
                return Result::BACK;

            default:
                break;
        }
    }
}

const char* OnlineLevelBrowser::GetSelectedPath()
{
    return selectedPath;
}
