/**

* Author: Daud Iqbal

* Assignment: Rise of the AI

* Date due: 11/16/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/LevelD.h"
#include "CS3113/TextScene.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 576,
              FPS           = 120;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr int MAX_LIVES = 3;
constexpr float HUD_SCALE = 3.0f,
                HUD_MARGIN = 20.0f,
                HUD_SPACING = 8.0f; 

constexpr float HEART_SCALE = 2.0f;

constexpr float RESPAWN_FLASH_TIME = 2.0f;
constexpr float MUSIC_VOLUME = 0.33f;

constexpr int   HERO_DEATH_FRAME_SPEED = 8;
constexpr float HERO_DEATH_TIME = 1.9f;

// lose screen drawn
constexpr Color LOSE_TINT = { 200, 70, 80, 255 },
                MENU_TINT = { 170, 170, 180, 255 };

// screen shake effect
constexpr float SHAKE_TIME = 0.3f,
                SHAKE_STRENGTH = 6.0f,
                SHAKE_SPEED = 60.0f;

// boss health bar, at the bottom in the middle
constexpr float BOSS_BAR_WIDTH  = 300.0f,
                BOSS_BAR_HEIGHT = 14.0f,
                BOSS_BAR_TOP = SCREEN_HEIGHT - 36.0f,
                BOSS_NAME_TOP = BOSS_BAR_TOP - 30.0f;

constexpr Color BOSS_BAR_BACK = {  40,  10,  16, 255 },
                BOSS_BAR_FILL = { 200,  48,  48, 255 };

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;


int gLives = MAX_LIVES;
Texture2D gHeartTexture = {};
Texture2D gHeartEmptyTexture = {};
Texture2D gKeyTexture = {};
Texture2D gBossNameTexture = {};

float gShakeTimer = 0.0f;
int   gLastBossHealth = 0;

Sound gHurtSound = {};
Music gGameMusic = {};
Sound gDeathSound = {};
Sound gDoorSound = {};

Entity *gHeroDeath  = nullptr;
float gDeathTimer = 0.0f;

Camera2D gCamera = {};

Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene *> gLevels = {};

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    bool wasInLevel = gCurrentScene != nullptr && gCurrentScene->getState().player != nullptr;

    if (gCurrentScene) gCurrentScene->shutdown();
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCamera.target = ORIGIN;

    // a new scene never starts in the middle of a screen shake
    gShakeTimer = 0.0f;
    gCamera.offset  = ORIGIN;
    gLastBossHealth = gCurrentScene->getState().bossHealth;

    // how music works in the gmae
    bool isLevel = gCurrentScene->getState().player != nullptr;

    if (isLevel && !wasInLevel) PlayMusicStream(gGameMusic);
    if (!isLevel) StopMusicStream(gGameMusic);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crypt Escape");
    InitAudioDevice();

    gHeartTexture = LoadTexture("assets/game/heart.png");
    gHeartEmptyTexture = LoadTexture("assets/game/heart_empty.png");
    gKeyTexture = LoadTexture("assets/game/key.png");
    gBossNameTexture = LoadTexture("assets/game/boss_name.png");

    gHurtSound = LoadSound("assets/game/hurt.wav");
    gDeathSound = LoadSound("assets/game/death.wav");
    gDoorSound = LoadSound("assets/game/door.wav");

    gGameMusic = LoadMusicStream("assets/game/music.ogg");
    SetMusicVolume(gGameMusic, MUSIC_VOLUME);

    // death
    std::map<Direction, std::vector<int>> heroDeathAtlas = {
        { RIGHT, {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11, 11, 11 } },
        { LEFT,  { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23 } },
    };

    gHeroDeath = new Entity(
        ORIGIN,                                         // position, set when he dies
        { 48.0f * SPRITE_SCALE, 40.0f * SPRITE_SCALE }, // scale
        "assets/game/hero_death.png",                   // texture file address
        ATLAS,                                          // single image or atlas?
        { 2, 12 },                                      // atlas dimensions
        heroDeathAtlas,                                 // actual atlas
        NONE                                            // entity type
    );

    gHeroDeath->setFrameSpeed(HERO_DEATH_FRAME_SPEED);
    gHeroDeath->deactivate();

    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#0E0B14");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#0B0E14");
    gLevels[LEVEL_C] = new LevelC(ORIGIN, "#0D0A12");
    gLevels[LEVEL_D] = new LevelD(ORIGIN, "#120A0E");

    gLevels[MENU] = new TextScene(ORIGIN, "#0B0910", "assets/game/bg_castle.png",   "assets/game/title_menu.png", "assets/game/prompt_start.png", MENU_TINT,
                                "assets/game/menu_music.wav", true);
    gLevels[WIN] = new TextScene(ORIGIN, "#0B0910", "assets/game/bg_castle.png",   "assets/game/title_win.png",  "assets/game/prompt_menu.png",  WHITE,
                                "assets/game/victory_sting.ogg", false);
    gLevels[LOSE] = new TextScene(ORIGIN, "#0B0910", "assets/game/bg_corridor.png", "assets/game/title_lose.png", "assets/game/prompt_menu.png",  LOSE_TINT,
                                "assets/game/game_over.ogg", false);

    switchToScene(gLevels[MENU]);

    gCamera.offset = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 1.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    
    if (gHeroDeath->isActive()) return;

    // for debugging purposes or could be a cheat!
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) ||
        IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_FOUR)) {
        if (gLives <= 0) gLives = MAX_LIVES;

        if (IsKeyPressed(KEY_ONE)) switchToScene(gLevels[LEVEL_A]);
        else if (IsKeyPressed(KEY_TWO)) switchToScene(gLevels[LEVEL_B]);
        else if (IsKeyPressed(KEY_THREE)) switchToScene(gLevels[LEVEL_C]);
        else switchToScene(gLevels[LEVEL_D]);
    }

    // za menu
    if (gCurrentScene->getState().player == nullptr) {
        if (IsKeyPressed(KEY_ENTER)) {
            gLives = MAX_LIVES; // start with max lives
            if (gCurrentScene == gLevels[MENU]) { switchToScene(gLevels[LEVEL_A]); }
            else switchToScene(gLevels[MENU]); }

        return; }
    
    gCurrentScene->getState().player->resetMovement();

    if (IsKeyDown(KEY_A)) gCurrentScene->getState().player->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().player->moveRight();

    if (IsKeyPressed(KEY_W) &&
        gCurrentScene->getState().player->isCollidingBottom()) {
        gCurrentScene->getState().player->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().player->getMovement()) > 1.0f) {
        gCurrentScene->getState().player->normaliseMovement(); 
    }
}

void update()
{
    // Delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // keep the music streaming
    UpdateMusicStream(gGameMusic);

    deltaTime += gTimeAccumulator;
    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        // when hero dies
        if (gHeroDeath->isActive())
        {
            gHeroDeath->update(FIXED_TIMESTEP, nullptr, nullptr, nullptr, 0);
            gDeathTimer -= FIXED_TIMESTEP;
            deltaTime   -= FIXED_TIMESTEP;
            continue;
        }

        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
        
        if (gCurrentScene->getState().player == nullptr) continue;

        Vector2 currentPlayerPosition = {
            gCurrentScene->getState().player->getPosition().x, ORIGIN.y
        };

        Map *map = gCurrentScene->getState().map;
        float minCameraX = map->getLeftBoundary()  + ORIGIN.x;
        float maxCameraX = map->getRightBoundary() - ORIGIN.x;

        if (currentPlayerPosition.x < minCameraX) currentPlayerPosition.x = minCameraX;
        if (currentPlayerPosition.x > maxCameraX) currentPlayerPosition.x = maxCameraX;

        panCamera(&gCamera, &currentPlayerPosition);

        if (gShakeTimer > 0.0f)
        {
            gShakeTimer -= FIXED_TIMESTEP;

            gCamera.offset = {
                ORIGIN.x + SHAKE_STRENGTH * cos(gShakeTimer * SHAKE_SPEED),
                ORIGIN.y + SHAKE_STRENGTH * cos(gShakeTimer * SHAKE_SPEED * 1.5f)
            };
        }
        else gCamera.offset = ORIGIN;
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();
    gHeroDeath->render(); // only when the payer dies

    EndMode2D();
    
    if (gCurrentScene->getState().player != nullptr)
    {
        float heartWidth  = gHeartTexture.width  * HEART_SCALE;
        float heartHeight = gHeartTexture.height * HEART_SCALE;

        for (int i = 0; i < MAX_LIVES; i++)
        {
            Texture2D heart = (i < gLives) ? gHeartTexture : gHeartEmptyTexture;

            DrawTexturePro(
                heart,
                { 0.0f, 0.0f,
                  static_cast<float>(heart.width),
                  static_cast<float>(heart.height) },
                { HUD_MARGIN + i * (heartWidth + HUD_SPACING), HUD_MARGIN,
                  heartWidth, heartHeight },
                { 0.0f, 0.0f }, 0.0f, WHITE
            );
        }
        if (gCurrentScene->getState().hasKey)
        {
            float keySize = gKeyTexture.width * HUD_SCALE;

            DrawTexturePro(
                gKeyTexture,
                { 0.0f, 0.0f,
                  static_cast<float>(gKeyTexture.width),
                  static_cast<float>(gKeyTexture.height) },
                { HUD_MARGIN + MAX_LIVES * (heartWidth + HUD_SPACING),
                  HUD_MARGIN + heartHeight / 2.0f - keySize / 2.0f,
                  keySize, keySize },
                { 0.0f, 0.0f }, 0.0f, WHITE
            );
        }

        int bossHealth = gCurrentScene->getState().bossHealth;

        if (bossHealth > 0)
        {
            float nameWidth  = gBossNameTexture.width  * HUD_SCALE;
            float nameHeight = gBossNameTexture.height * HUD_SCALE;

            DrawTexturePro(
                gBossNameTexture,
                { 0.0f, 0.0f,
                  static_cast<float>(gBossNameTexture.width),
                  static_cast<float>(gBossNameTexture.height) },
                { ORIGIN.x - nameWidth / 2.0f, BOSS_NAME_TOP, nameWidth, nameHeight },
                { 0.0f, 0.0f }, 0.0f, WHITE
            );

            float barLeft = ORIGIN.x - BOSS_BAR_WIDTH / 2.0f;
            float filled  = BOSS_BAR_WIDTH * bossHealth / LevelD::BOSS_MAX_HEALTH;

            DrawRectangle(barLeft, BOSS_BAR_TOP, BOSS_BAR_WIDTH, BOSS_BAR_HEIGHT, BOSS_BAR_BACK);
            DrawRectangle(barLeft, BOSS_BAR_TOP, filled, BOSS_BAR_HEIGHT, BOSS_BAR_FILL);
            DrawRectangleLines(barLeft, BOSS_BAR_TOP, BOSS_BAR_WIDTH, BOSS_BAR_HEIGHT, BLACK);
        }
    }

    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();

    UnloadTexture(gHeartTexture);
    UnloadTexture(gHeartEmptyTexture);
    UnloadSound(gHurtSound);
    UnloadSound(gDeathSound);
    UnloadSound(gDoorSound);
    UnloadMusicStream(gGameMusic);
    UnloadTexture(gKeyTexture);
    UnloadTexture(gBossNameTexture);
    
    delete gHeroDeath;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        int bossHealth = gCurrentScene->getState().bossHealth;

        if (bossHealth < gLastBossHealth)
            gShakeTimer = (bossHealth == 0) ? SHAKE_TIME * 3.0f : SHAKE_TIME;

        gLastBossHealth = bossHealth;

        bool fellInPit = gCurrentScene->getState().playerDied;
        bool hitByEnemy = gCurrentScene->getState().playerHit;

        if (gHeroDeath->isActive()) {
            if (gDeathTimer <= 0.0f) {
                gHeroDeath->deactivate();
                switchToScene(gLevels[LOSE]);
            }
        }
        else if (fellInPit || hitByEnemy) {
            gLives--;
            if (gLives <= 0 && hitByEnemy) {
                Entity *player = gCurrentScene->getState().player;

                gHeroDeath->setPosition({
                    player->getPosition().x,
                    player->getPosition().y + player->getScale().y / 2.0f
                                            - gHeroDeath->getScale().y / 2.0f
                });
                gHeroDeath->setDirection(player->getDirection());
                gHeroDeath->resetAnimation();
                gHeroDeath->activate();
                player->deactivate();

                gDeathTimer = HERO_DEATH_TIME;
                PlaySound(gDeathSound);
            }
            else if (gLives <= 0) {
                PlaySound(gDeathSound);
                switchToScene(gLevels[LOSE]);
            }
            else {
                PlaySound(gHurtSound);
                if (fellInPit) switchToScene(gCurrentScene);
                else gCurrentScene->clearPlayerHit();

                gCurrentScene->getState().player->startFlashing(RESPAWN_FLASH_TIME);
            }
        } else if (gCurrentScene->getState().nextSceneID != NO_SCENE) {
            SceneID id = gCurrentScene->getState().nextSceneID;
            PlaySound(gDoorSound);
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}
