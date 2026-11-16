#include "LevelA.h"

LevelA::LevelA()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
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

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,          // map grid cols & rows
        mLevelData,                         // grid data
        "assets/game/dungeon_tiles.png",    // texture filepath
        TILE_DIMENSION,                     // tile size
        17, 52,                             // texture cols & rows
        mapOrigin                           // in-game origin
    );

    // Player
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

    // standing still: loop the idle frames instead of freezing on a walk frame
    mGameState.player->setIdleAtlas({
        { RIGHT, { 18, 19, 20, 21, 22, 23, 24, 25 } },
        { LEFT,  { 27, 28, 29, 30, 31, 32, 33, 34 } },
    });

    // enemies / entities 
    std::map<Direction, std::vector<int>> slimeAnimationAtlas = {
        { RIGHT, { 0, 1,  2,  3,  4,  5,  6 } },
        { LEFT,  { 7, 8,  9, 10, 11, 12, 13 } },
    };

    Vector2 slimeScale = { 32.0f * SPRITE_SCALE, 12.0f * SPRITE_SCALE };

    Vector2 slimeSpawns[ENEMY_COUNT] = {
        { 21.5f * TILE_DIMENSION, 7 * TILE_DIMENSION }, // on top of the raised step
        { 34.5f * TILE_DIMENSION, groundTop          }  // guarding the last stretch
    };

    for (int i = 0; i < ENEMY_COUNT; i++)
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

    // some decorations for the layout
    std::map<Direction, std::vector<int>> torchAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 3 } },
        { LEFT,  { 0, 1, 2, 3 } },
    };

    Vector2 torchScale  = { 21.0f * SPRITE_SCALE, 27.0f * SPRITE_SCALE };
    Vector2 statueScale = { 32.0f * SPRITE_SCALE, 32.0f * SPRITE_SCALE };
    Vector2 candleScale = { 16.0f * SPRITE_SCALE, 16.0f * SPRITE_SCALE };

    // hang torches
    float torchSpawns[TORCH_COUNT] = { 3.5f, 10.0f, 16.5f, 21.5f, 30.0f, 35.0f };

    // statue decoration
    float statueSpawns[STATUE_COUNT] = { 10.5f, 33.0f };

    // candles
    Vector2 candleSpawns[CANDLE_COUNT] = {
        { 7.0f, 9 },
        { 16.0f, 9 },
        { 20.0f, 7 },
        { 26.0f, 9 },
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

    // Key and the door
    Vector2 keyScale  = { 16.0f * SPRITE_SCALE, 16.0f * SPRITE_SCALE };
    Vector2 doorScale = { 32.0f * SPRITE_SCALE, 48.0f * SPRITE_SCALE };

    // placing the key
    mKey = new Entity(
        { 26.5f * TILE_DIMENSION, 4 * TILE_DIMENSION - keyScale.y / 2.0f }, // position
        keyScale,                                                           // scale
        "assets/game/key.png",                                              // texture file address
        NONE                                                                // entity type
    );

    // keep the door locked until key pciked
    Vector2 doorPosition = { 38.0f * TILE_DIMENSION, groundTop - doorScale.y / 2.0f };

    mDoorClosed = new Entity(doorPosition, doorScale, "assets/game/door_closed.png", NONE);
    mDoorOpen   = new Entity(doorPosition, doorScale, "assets/game/door_open.png",   NONE);

    // middle door allows player to move on
    mDoorOpen->setColliderDimensions({ TILE_DIMENSION, doorScale.y * 0.8f });
    mDoorOpen->deactivate();
}

void LevelA::update(float deltaTime)
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

        // touching an enemy costs a heart
        if (!mGameState.player->isFlashing() && mGameState.player->isColliding(mEnemies[i]))
            mGameState.playerHit = true;
    }

    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;

    // pick key door unlocks
    if (!mGameState.hasKey && mGameState.player->isColliding(mKey))
    {
        mGameState.hasKey = true;
        mKey->deactivate();
        mDoorClosed->deactivate();
        mDoorOpen->activate();
        PlaySound(mGameState.keySound);
    }

    if (mGameState.player->isColliding(mDoorOpen))
        mGameState.nextSceneID = LEVEL_B;
}

void LevelA::render()
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

    // scenery first, so the map, enemies and player are drawn in front of it
    for (int i = 0; i < DECORATION_COUNT; i++) mDecorations[i]->render();

    mGameState.map->render();
    mDoorClosed->render();
    mDoorOpen->render();
    mKey->render();

    for (int i = 0; i < ENEMY_COUNT; i++) mEnemies[i]->render();
    mGameState.player->render();
}

void LevelA::shutdown()
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
