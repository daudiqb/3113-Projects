#ifndef LEVELB_H
#define LEVELB_H

#include "LevelA.h"

class LevelB : public Scene
{
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 163, 164, 165,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 163, 164, 165,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,  36,  36,  36,   0,   0,  69,
        69,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,  69,  69,  69,   0,   0,  69,
        69,  36,  36,  36,  36,  36,  36,  36,  36,   0,   0,   0,  36,  36,  36,  36,  36,  36,  36,  36,   0,   0,   0,   0,  36,  36,  36,  36,  36,  36,  36,  36,  36,  69,  69,  69,  69,  36,  36,  69,
        69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,
        69,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,   0,   0,   0,   0,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69,  69
    };

    static constexpr int ENEMY_COUNT = 3;
    Entity *mEnemies[ENEMY_COUNT] = {};
    
    Entity *mKey        = nullptr;
    Entity *mDoorClosed = nullptr;
    Entity *mDoorOpen   = nullptr;

    static constexpr int TORCH_COUNT      = 6,
                         STATUE_COUNT     = 2,
                         CANDLE_COUNT     = 5,
                         DECORATION_COUNT = TORCH_COUNT + STATUE_COUNT + CANDLE_COUNT;
    Entity *mDecorations[DECORATION_COUNT] = {};

public:
    static constexpr float TILE_DIMENSION          = 16.0f * SPRITE_SCALE,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD      = 800.0f;

    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELB_H
