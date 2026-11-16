#ifndef LEVELA_H
#define LEVELA_H

#include "Scene.h"

constexpr int LEVEL_WIDTH  = 40,
              LEVEL_HEIGHT = 12;

constexpr float SPRITE_SCALE = 3.0f;

constexpr int SLIME_SPEED = 60;
constexpr int   SKELETON_SPEED = 110;
constexpr float BAT_LERP_FACTOR = 0.35f; 
constexpr float BAT_SCALE = 5.0f;

constexpr float TORCH_ROW         = 4.5f;
constexpr int   TORCH_FRAME_SPEED = 8;

class LevelA : public Scene
{
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 163, 164, 165,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0, 163, 164, 165,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,  36,  36,  36,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,  69,  69,  69,  69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,   0,   0,   0,  36,  36,  36,  36,  69,  69,  69,  69,  69,  36,  36,  36,  36,   0,   0,   0,  36,  36,  36,  36,  36,  36,  36,  36,  69,
        69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69,
        69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69
    };


    static constexpr int ENEMY_COUNT = 2;
    Entity *mEnemies[ENEMY_COUNT] = {};

    Entity *mKey        = nullptr;
    Entity *mDoorClosed = nullptr;
    Entity *mDoorOpen   = nullptr;

    // scenery as to how its going to look
    static constexpr int TORCH_COUNT      = 6,
                         STATUE_COUNT     = 2,
                         CANDLE_COUNT     = 5,
                         DECORATION_COUNT = TORCH_COUNT + STATUE_COUNT + CANDLE_COUNT;
    Entity *mDecorations[DECORATION_COUNT] = {};

public:
    static constexpr float TILE_DIMENSION          = 16.0f * SPRITE_SCALE,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD      = 800.0f;

    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELA_H
