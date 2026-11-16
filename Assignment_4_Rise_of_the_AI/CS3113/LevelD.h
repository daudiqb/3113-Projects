#ifndef LEVELD_H
#define LEVELD_H

#include "LevelA.h"

constexpr int BOSS_LEVEL_WIDTH = 26;

constexpr Color BOSS_ROOM_TINT = { 190, 120, 130, 255 }; // make it glow a but red

class LevelD : public Scene
{
private:
    unsigned int mLevelData[BOSS_LEVEL_WIDTH * LEVEL_HEIGHT] = {
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
         69,   0,   0,   0,   0,   0,   0,  36,  36,  36,   0,   0,  36,  36,  36,   0,   0,  36,  36,  36,   0,   0,   0,   0,   0,  69,
         69,  36,  36,  36,  36,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  36,  36,  36,  69,
         69,  69,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,  69,
         69,  69,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,   0,   0,  69,  69,  69,  69
    };

    // bridge
    static constexpr int BRIDGE_START  = 6,  
                         BRIDGE_LENGTH = 15,
                         BRIDGE_ROW    = 3,
                         CRACKED_COUNT = 3,
                         BLOCK_COUNT   = BRIDGE_LENGTH;
    Entity *mBridge = nullptr;
    int     mCrackedSlots[CRACKED_COUNT] = {};

    Entity *mExplosions[CRACKED_COUNT] = {};
    float   mExplosionTimers[CRACKED_COUNT] = {};
    float   mRespawnTimers[CRACKED_COUNT] = {};

    Entity *mBoss = nullptr;

    // rocks the boss knocks loose from the bridge
    static constexpr int ROCK_COUNT = 4;
    Entity *mRocks[ROCK_COUNT] = {};
    float   mRockTimer = 0.0f;

    Entity *mDoorClosed = nullptr;
    Entity *mDoorOpen   = nullptr;

    static constexpr int TORCH_COUNT      = 4,
                         STATUE_COUNT     = 1,
                         CANDLE_COUNT     = 5,
                         DECORATION_COUNT = TORCH_COUNT + STATUE_COUNT + CANDLE_COUNT;
    Entity *mDecorations[DECORATION_COUNT] = {};

    Sound mBreakSound    = {};
    Sound mBossHitSound  = {};
    Sound mRockSound     = {};
    Sound mCollapseSound = {};
    Sound mVictorySound  = {};

public:
    static constexpr float TILE_DIMENSION = 16.0f * SPRITE_SCALE,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD  = 800.0f;

    static constexpr int   BOSS_MAX_HEALTH    = CRACKED_COUNT,
                           BOSS_SPEED         = 70,
                           BOSS_SPEED_STEP    = 25;
    static constexpr float BOSS_FLASH_TIME    = 0.8f,
                           ROCK_INTERVAL      = 1.6f,
                           ROCK_INTERVAL_STEP = 0.35f, 
                           ROCK_GRAVITY       = 500.0f, 
                           BLOCK_RESPAWN_TIME = 5.0f,
                           EXPLOSION_TIME     = 0.55f;

    static constexpr int   EXPLOSION_FRAME_SPEED = 14;

    LevelD();
    LevelD(Vector2 origin, const char *bgHexCode);
    ~LevelD();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELD_H
