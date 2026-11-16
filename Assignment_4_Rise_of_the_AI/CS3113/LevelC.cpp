#include "LevelC.h"

LevelC::LevelC()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelC::LevelC(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.playerDied  = false;
    mGameState.playerHit   = false;
    mGameState.hasKey      = false;

    mGameState.jumpSound = LoadSound("assets/game/jump.wav");
    mGameState.keySound  = LoadSound("assets/game/key.wav");
    mGameState.background = LoadTexture("assets/game/background.png");

    Vector2 mapOrigin = {
        LEVEL_WIDTH  * TILE_DIMENSION / 2.0f,
        LEVEL_HEIGHT * TILE_DIMENSION / 2.0f
    };

    // map
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,          // map grid cols & rows
        mLevelData,                         // grid data
        "assets/game/dungeon_tiles.png",    // texture filepath
        TILE_DIMENSION,                     // tile size
        17, 52,                             // texture cols & rows
        mapOrigin                           // in-game origin
    );

    // player
    std::map<Direction, std::vector<int>> playerAnimationAtlas = {
        { RIGHT, {  0,  1,  2,  3,  4,  5,  6,  7,  8 } },
        { LEFT,  {  9, 10, 11, 12, 13, 14, 15, 16, 17 } },
    };

    Vector2 playerScale = { 32.0f * SPRITE_SCALE, 32.0f * SPRITE_SCALE };

    float groundTop = 9 * TILE_DIMENSION;

    mGameState.player = new Entity(
        { 2.5f * TILE_DIMENSION, groundTop - playerScale.y / 2.0f },    // position
        playerScale,                                                    // scale
        "assets/game/hero.png",                                         // texture file address
        ATLAS,                                                          // single image or atlas?
        { 4, 9 },                                                       // atlas dimensions
        playerAnimationAtlas,                                           // actual atlas
        PLAYER                                                          // entity type
    );

    mGameState.player->setJumpingPower(550.0f);
    mGameState.player->setColliderDimensions({
        playerScale.x / 2.0f,
        playerScale.y - 2.0f * SPRITE_SCALE
    });
    mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    mGameState.player->setIdleAtlas({
        { RIGHT, { 18, 19, 20, 21, 22, 23, 24, 25 } },
        { LEFT,  { 27, 28, 29, 30, 31, 32, 33, 34 } },
    });

    // all the enemies
    std::map<Direction, std::vector<int>> slimeAnimationAtlas = {
        { RIGHT, { 0, 1,  2,  3,  4,  5,  6 } },
        { LEFT,  { 7, 8,  9, 10, 11, 12, 13 } },
    };
    std::map<Direction, std::vector<int>> skeletonAnimationAtlas = {
        { RIGHT, { 0, 1, 2,  3,  4,  5 } },
        { LEFT,  { 6, 7, 8,  9, 10, 11 } },
    };

    std::map<Direction, std::vector<int>> batAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 3 } },
        { LEFT,  { 0, 1, 2, 3 } },
    };

    Vector2 slimeScale    = { 32.0f * SPRITE_SCALE, 12.0f * SPRITE_SCALE };
    Vector2 skeletonScale = { 32.0f * SPRITE_SCALE, 27.0f * SPRITE_SCALE };
    Vector2 batScale      = { 16.0f * BAT_SCALE,    16.0f * BAT_SCALE    };

    Vector2 slimeSpawns[SLIME_COUNT] = {
        { 10.5f * TILE_DIMENSION, groundTop          },
        { 23.5f * TILE_DIMENSION, 7 * TILE_DIMENSION }
    };

    float skeletonSpawns[SKELETON_COUNT] = {
        26.5f * TILE_DIMENSION,
        35.5f * TILE_DIMENSION
    };

    // bats: where they hang, high up
    Vector2 batSpawns[BAT_COUNT] = {
        { 11.0f * TILE_DIMENSION, 1.5f * TILE_DIMENSION },
        { 33.0f * TILE_DIMENSION, 1.5f * TILE_DIMENSION }
    };

    for (int i = 0; i < SLIME_COUNT; i++)
    {
        mEnemies[i] = new Entity(
            { slimeSpawns[i].x, slimeSpawns[i].y - slimeScale.y / 2.0f }, // position
            slimeScale,                                                   // scale
            "assets/game/slime.png",                                      // texture file address
            ATLAS,                                                        // single image or atlas?
            { 4, 7 },                                                     // atlas dimensions
            slimeAnimationAtlas,                                          // actual atlas
            NPC                                                           // entity type
        );

        mEnemies[i]->setAIType(WANDERER);
        mEnemies[i]->setSpeed(SLIME_SPEED);
        mEnemies[i]->setColliderDimensions({ slimeScale.x * 0.6f, slimeScale.y });
        mEnemies[i]->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    }

    for (int i = 0; i < SKELETON_COUNT; i++)
    {
        int slot = SLIME_COUNT + i;

        mEnemies[slot] = new Entity(
            { skeletonSpawns[i], groundTop - skeletonScale.y / 2.0f }, // position
            skeletonScale,                                             // scale
            "assets/game/skeleton.png",                                // texture file address
            ATLAS,                                                     // single image or atlas?
            { 4, 6 },                                                  // atlas dimensions
            skeletonAnimationAtlas,                                    // actual atlas
            NPC                                                        // entity type
        );

        mEnemies[slot]->setAIType(FOLLOWER);
        mEnemies[slot]->setAIState(IDLE);
        mEnemies[slot]->setSpeed(SKELETON_SPEED);
        mEnemies[slot]->setColliderDimensions({
            skeletonScale.x * 0.45f,
            skeletonScale.y - 2.0f * SPRITE_SCALE
        });
        mEnemies[slot]->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
        mEnemies[slot]->setDirection(LEFT);
        mEnemies[slot]->setIdleAtlas({
            { RIGHT, { 12, 13, 14, 15, 16 } },
            { LEFT,  { 18, 19, 20, 21, 22 } },
        });
    }

    for (int i = 0; i < BAT_COUNT; i++)
    {
        int slot = SLIME_COUNT + SKELETON_COUNT + i;

        mEnemies[slot] = new Entity(
            batSpawns[i],          // position
            batScale,              // scale
            "assets/game/bat.png", // texture file address
            ATLAS,                 // single image or atlas?
            { 17, 1 },             // atlas dimensions
            batAnimationAtlas,     // actual atlas
            NPC                    // entity type
        );

        mEnemies[slot]->setAIType(LERPER);
        mEnemies[slot]->setAIState(IDLE);
        mEnemies[slot]->setLerpFactor(BAT_LERP_FACTOR);
        mEnemies[slot]->setColliderDimensions({ batScale.x * 0.5f, batScale.y * 0.4f });
        mEnemies[slot]->setDirection(LEFT);
    }

    // map decoration!!
    std::map<Direction, std::vector<int>> torchAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 3 } },
        { LEFT,  { 0, 1, 2, 3 } },
    };

    Vector2 torchScale  = { 21.0f * SPRITE_SCALE, 27.0f * SPRITE_SCALE };
    Vector2 statueScale = { 32.0f * SPRITE_SCALE, 32.0f * SPRITE_SCALE };
    Vector2 candleScale = { 16.0f * SPRITE_SCALE, 16.0f * SPRITE_SCALE };

    float torchSpawns[TORCH_COUNT] = { 3.0f, 10.5f, 16.0f, 20.5f, 30.0f, 34.0f };

    float statueSpawns[STATUE_COUNT] = { 10.5f, 20.0f };

    Vector2 candleSpawns[CANDLE_COUNT] = {
        { 4.5f, 9 },
        { 12.3f, 9 },
        { 26.0f, 9 },
        { 33.0f, 9 },
        { 36.0f, 9 }
    };

    for (int i = 0; i < TORCH_COUNT; i++)
    {
        mDecorations[i] = new Entity(
            { torchSpawns[i] * TILE_DIMENSION, TORCH_ROW * TILE_DIMENSION }, // position
            torchScale,                                                      // scale
            "assets/game/torch.png",                                         // texture file address
            ATLAS,                                                           // single image or atlas?
            { 1, 4 },                                                        // atlas dimensions
            torchAnimationAtlas,                                             // actual atlas
            NONE                                                             // entity type
        );

        mDecorations[i]->setFrameSpeed(TORCH_FRAME_SPEED);
    }

    for (int i = 0; i < STATUE_COUNT; i++)
    {
        mDecorations[TORCH_COUNT + i] = new Entity(
            { statueSpawns[i] * TILE_DIMENSION, groundTop - statueScale.y / 2.0f }, // position
            statueScale,                                                            // scale
            "assets/game/statue.png",                                               // texture file address
            NONE                                                                    // entity type
        );
    }

    for (int i = 0; i < CANDLE_COUNT; i++)
    {
        mDecorations[TORCH_COUNT + STATUE_COUNT + i] = new Entity(
            { candleSpawns[i].x * TILE_DIMENSION,
              candleSpawns[i].y * TILE_DIMENSION - candleScale.y / 2.0f }, // position
            candleScale,                                                   // scale
            "assets/game/candles.png",                                     // texture file address
            NONE                                                           // entity type
        );
    }

    // za door and za key
    Vector2 keyScale  = { 16.0f * SPRITE_SCALE, 16.0f * SPRITE_SCALE };
    Vector2 doorScale = { 32.0f * SPRITE_SCALE, 48.0f * SPRITE_SCALE };

    mKey = new Entity(
        { 27.5f * TILE_DIMENSION, 4 * TILE_DIMENSION - keyScale.y / 2.0f }, // position
        keyScale,                                                           // scale
        "assets/game/key.png",                                              // texture file address
        NONE                                                                // entity type
    );

    Vector2 doorPosition = { 38.0f * TILE_DIMENSION, groundTop - doorScale.y / 2.0f };

    mDoorClosed = new Entity(doorPosition, doorScale, "assets/game/door_closed.png", NONE);
    mDoorOpen   = new Entity(doorPosition, doorScale, "assets/game/door_open.png",   NONE);

    mDoorOpen->setColliderDimensions({ TILE_DIMENSION, doorScale.y * 0.8f });
    mDoorOpen->deactivate();
}

void LevelC::update(float deltaTime)
{
    // animate torches
    for (int i = 0; i < DECORATION_COUNT; i++)
        mDecorations[i]->update(deltaTime, nullptr, nullptr, nullptr, 0);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    for (int i = 0; i < ENEMY_COUNT; i++) {
        mEnemies[i]->update(
            deltaTime,         // delta time / fixed timestep
            mGameState.player, // player, so the AI can react to it
            mGameState.map,    // map
            nullptr,           // collidable entities
            0                  // col. entity count
        );

        if (mEnemies[i]->getPosition().y > END_GAME_THRESHOLD) mEnemies[i]->deactivate();

        if (!mGameState.player->isFlashing() && mGameState.player->isColliding(mEnemies[i]))
            mGameState.playerHit = true;
    }

    // fell in pit lose life
    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;

    // picking key unlock door
    if (!mGameState.hasKey && mGameState.player->isColliding(mKey))
    {
        mGameState.hasKey = true;
        mKey->deactivate();
        mDoorClosed->deactivate();
        mDoorOpen->activate();
        PlaySound(mGameState.keySound);
    }

    if (mGameState.player->isColliding(mDoorOpen))
        mGameState.nextSceneID = LEVEL_D;
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    DrawTexturePro(
        mGameState.background,
        { 0.0f, 0.0f,
          static_cast<float>(mGameState.background.width),
          static_cast<float>(mGameState.background.height) },
        { 0.0f, 0.0f,
          mGameState.background.width  * SPRITE_SCALE,
          mGameState.background.height * SPRITE_SCALE },
        { 0.0f, 0.0f }, 0.0f, WHITE
    );

    for (int i = 0; i < DECORATION_COUNT; i++) mDecorations[i]->render();

    mGameState.map->render();
    mDoorClosed->render();
    mDoorOpen->render();
    mKey->render();

    for (int i = 0; i < ENEMY_COUNT; i++) mEnemies[i]->render();
    mGameState.player->render();
}

void LevelC::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    for (int i = 0; i < DECORATION_COUNT; i++) {
        delete mDecorations[i];
        mDecorations[i] = nullptr;
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        delete mEnemies[i];
        mEnemies[i] = nullptr;
    }

    delete mKey;
    mKey = nullptr;

    delete mDoorClosed;
    mDoorClosed = nullptr;

    delete mDoorOpen;
    mDoorOpen = nullptr;

    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};

    UnloadSound(mGameState.keySound);
    mGameState.keySound = {};

    UnloadTexture(mGameState.background);
    mGameState.background = {};
}
