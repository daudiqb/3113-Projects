#include "LevelD.h"

LevelD::LevelD()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelD::LevelD(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelD::~LevelD() { shutdown(); }

void LevelD::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.playerDied  = false;
    mGameState.playerHit   = false;
    mGameState.hasKey      = false;
    mGameState.bossHealth  = BOSS_MAX_HEALTH;

    mGameState.jumpSound = LoadSound("assets/game/jump.wav");
    mGameState.background = LoadTexture("assets/game/background.png");

    mBreakSound    = LoadSound("assets/game/break.wav");
    mBossHitSound  = LoadSound("assets/game/boss_hit.wav");
    mRockSound     = LoadSound("assets/game/rock.wav");
    mCollapseSound = LoadSound("assets/game/collapse.wav");
    mVictorySound  = LoadSound("assets/game/victory.wav");

    mRockTimer = 0.0f;

    for (int i = 0; i < CRACKED_COUNT; i++)
    {
        mExplosionTimers[i] = 0.0f;
        mRespawnTimers[i]   = 0.0f;
    }

    Vector2 mapOrigin = {
        BOSS_LEVEL_WIDTH * TILE_DIMENSION / 2.0f,
        LEVEL_HEIGHT     * TILE_DIMENSION / 2.0f
    };

    // map
    mGameState.map = new Map(
        BOSS_LEVEL_WIDTH, LEVEL_HEIGHT,     // map grid cols & rows
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

    // add the weak box
    int crackedColumns[CRACKED_COUNT] = { 8, 13, 18 };
    mBridge = new Entity[BLOCK_COUNT];

    for (int i = 0; i < BRIDGE_LENGTH; i++)
    {
        float column = static_cast<float>(BRIDGE_START + i);

        mBridge[i].setTexture("assets/game/block.png");
        mBridge[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        mBridge[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        mBridge[i].setPosition({
            (column + 0.5f) * TILE_DIMENSION, (BRIDGE_ROW + 0.5f) * TILE_DIMENSION
        });
        mBridge[i].setEntityType(BLOCK);
    }

    for (int i = 0; i < CRACKED_COUNT; i++)
    {
        int slot = crackedColumns[i] - BRIDGE_START;
        mCrackedSlots[i] = slot;

        // the orange blocks are the weak ones
        mBridge[slot].setTexture("assets/game/block_weak.png");
    }

    // explosion for the boss dmg mechanism 
    std::map<Direction, std::vector<int>> explosionAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6 } },
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6 } },
    };

    Vector2 explosionScale = { 32.0f * SPRITE_SCALE, 32.0f * SPRITE_SCALE };

    for (int i = 0; i < CRACKED_COUNT; i++)
    {
        mExplosions[i] = new Entity(
            mBridge[mCrackedSlots[i]].getPosition(), // position, on its weak block
            explosionScale,                          // scale
            "assets/game/explosion.png",             // texture file address
            ATLAS,                                   // single image or atlas?
            { 1, 7 },                                // atlas dimensions
            explosionAnimationAtlas,                 // actual atlas
            NONE                                     // entity type
        );

        // the blast reaches as far as the explosion is drawn
        mExplosions[i]->setColliderDimensions(explosionScale);
        mExplosions[i]->setFrameSpeed(EXPLOSION_FRAME_SPEED);
        mExplosions[i]->deactivate();
    }

    // the boss walks along the bridge always right above the player
    std::map<Direction, std::vector<int>> bossAnimationAtlas = {
        { RIGHT, { 0, 1,  2,  3,  4,  5,  6,  7 } },
        { LEFT,  { 8, 9, 10, 11, 12, 13, 14, 15 } },
    };

    Vector2 bossScale    = { 100.0f * SPRITE_SCALE, 48.0f * SPRITE_SCALE };
    Vector2 bossCollider = { 32.0f * SPRITE_SCALE, bossScale.y - 2.0f * SPRITE_SCALE };

    mBoss = new Entity(
        { 13.5f * TILE_DIMENSION, BRIDGE_ROW * TILE_DIMENSION - bossCollider.y / 2.0f }, // position
        bossScale,                                                                       // scale
        "assets/game/boss.png",                                                          // texture file address
        ATLAS,                                                                           // single image or atlas?
        { 4, 8 },                                                                        // atlas dimensions
        bossAnimationAtlas,                                                              // actual atlas
        NPC                                                                              // entity type
    );

    mBoss->setAIType(FOLLOWER);
    mBoss->setAIState(WALKING);
    mBoss->setSpeed(BOSS_SPEED);
    mBoss->setColliderDimensions(bossCollider);
    mBoss->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    mBoss->setDirection(LEFT);
    mBoss->setIdleAtlas({
        { RIGHT, { 16, 17, 18, 19, 20 } },
        { LEFT,  { 24, 25, 26, 27, 28 } },
    });

    // boss weapon
    Vector2 rockScale = { 16.0f * SPRITE_SCALE, 11.0f * SPRITE_SCALE };

    for (int i = 0; i < ROCK_COUNT; i++)
    {
        mRocks[i] = new Entity(
            { 0.0f, 0.0f },          // position, set when the rock drops
            rockScale,               // scale
            "assets/game/rock.png",  // texture file address
            NONE                     // entity type
        );

        mRocks[i]->setColliderDimensions({ rockScale.x * 0.7f, rockScale.y * 0.6f });
        mRocks[i]->setAcceleration({ 0.0f, ROCK_GRAVITY });
        mRocks[i]->deactivate();
    }

    // map decorations!!
    std::map<Direction, std::vector<int>> torchAnimationAtlas = {
        { RIGHT, { 0, 1, 2, 3 } },
        { LEFT,  { 0, 1, 2, 3 } },
    };

    Vector2 torchScale  = { 21.0f * SPRITE_SCALE, 27.0f * SPRITE_SCALE };
    Vector2 statueScale = { 32.0f * SPRITE_SCALE, 32.0f * SPRITE_SCALE };
    Vector2 candleScale = { 16.0f * SPRITE_SCALE, 16.0f * SPRITE_SCALE };

    float torchSpawns[TORCH_COUNT] = { 2.0f, 10.5f, 15.5f, 22.0f };

    float statueSpawns[STATUE_COUNT] = { 13.0f };

    Vector2 candleSpawns[CANDLE_COUNT] = {
        { 1.5f, 9 }, { 4.0f, 9 }, { 8.0f, 8 }, { 18.0f, 8 }, { 22.5f, 9 }
    };

    float pillarTop = 8 * TILE_DIMENSION;

    for (int i = 0; i < TORCH_COUNT; i++)
    {
        mDecorations[i] = new Entity(
            { torchSpawns[i] * TILE_DIMENSION, 5.5f * TILE_DIMENSION }, // position
            torchScale,                                                 // scale
            "assets/game/torch.png",                                    // texture file address
            ATLAS,                                                      // single image or atlas?
            { 1, 4 },                                                   // atlas dimensions
            torchAnimationAtlas,                                        // actual atlas
            NONE                                                        // entity type
        );

        mDecorations[i]->setFrameSpeed(TORCH_FRAME_SPEED);
    }

    for (int i = 0; i < STATUE_COUNT; i++)
    {
        mDecorations[TORCH_COUNT + i] = new Entity(
            { statueSpawns[i] * TILE_DIMENSION, pillarTop - statueScale.y / 2.0f }, // position
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

    // za door killing boss opens it
    Vector2 doorScale    = { 32.0f * SPRITE_SCALE, 48.0f * SPRITE_SCALE };
    Vector2 doorPosition = { 24.0f * TILE_DIMENSION, groundTop - doorScale.y / 2.0f };

    mDoorClosed = new Entity(doorPosition, doorScale, "assets/game/door_closed.png", NONE);
    mDoorOpen   = new Entity(doorPosition, doorScale, "assets/game/door_open.png",   NONE);

    mDoorOpen->setColliderDimensions({ TILE_DIMENSION, doorScale.y * 0.8f });
    mDoorOpen->deactivate();
}

void LevelD::update(float deltaTime)
{
    // animate torches
    for (int i = 0; i < DECORATION_COUNT; i++)
        mDecorations[i]->update(deltaTime, nullptr, nullptr, nullptr, 0);

    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        mBridge,        // collidable entities
        BLOCK_COUNT     // col. entity count
    );

    // jumping into a weak block from below makes it explode
    if (mGameState.bossHealth > 0 && mGameState.player->isCollidingTop())
    {
        float playerHalfWidth = mGameState.player->getColliderDimensions().x / 2.0f;

        for (int i = 0; i < CRACKED_COUNT; i++)
        {
            Entity *weak = &mBridge[mCrackedSlots[i]];
            if (!weak->isActive()) continue;

            float xGap = fabs(weak->getPosition().x - mGameState.player->getPosition().x) -
                         (TILE_DIMENSION / 2.0f + playerHalfWidth);
            if (xGap >= 0.0f) continue;

            weak->deactivate();

            mExplosions[i]->resetAnimation();
            mExplosions[i]->activate();
            mExplosionTimers[i] = EXPLOSION_TIME;
            PlaySound(mBreakSound);

            if (mExplosions[i]->isColliding(mBoss))
            {
                // the boss was standing in the blast he gets hurt and the
                // block is gone for good
                mGameState.bossHealth--;
                mBoss->startFlashing(BOSS_FLASH_TIME);
                mBoss->setSpeed(BOSS_SPEED + (BOSS_MAX_HEALTH - mGameState.bossHealth) * BOSS_SPEED_STEP);
                PlaySound(mBossHitSound);
            }
            else
            {
                // if the blast miss the boss respawn block
                mRespawnTimers[i] = BLOCK_RESPAWN_TIME;
            }

            break;
        }

        // hiting the boss 3 times removes the bridge and prompts him to die
        if (mGameState.bossHealth == 0)
        {
            for (int i = 0; i < BLOCK_COUNT; i++) mBridge[i].deactivate();
            PlaySound(mCollapseSound);
        }
    }

    for (int i = 0; i < CRACKED_COUNT; i++)
    {
        if (mExplosions[i]->isActive())
        {
            mExplosions[i]->update(deltaTime, nullptr, nullptr, nullptr, 0);

            mExplosionTimers[i] -= deltaTime;
            if (mExplosionTimers[i] <= 0.0f) mExplosions[i]->deactivate();
        }

        if (mRespawnTimers[i] > 0.0f)
        {
            mRespawnTimers[i] -= deltaTime;

            if (mRespawnTimers[i] <= 0.0f && mGameState.bossHealth > 0)
            {
                mBridge[mCrackedSlots[i]].activate();
                mBridge[mCrackedSlots[i]].startFlashing(BOSS_FLASH_TIME);
            }
        }
    }

    mBoss->update(
        deltaTime,         // delta time / fixed timestep
        mGameState.player, // player, so the AI can follow it
        nullptr,           // map
        mBridge,           // collidable entities
        BLOCK_COUNT        // col. entity count
    );

    if (mGameState.bossHealth > 0)
    {
        // keep the boss on the bridge while it stands
        float halfWidth = mBoss->getColliderDimensions().x / 2.0f;
        float leftEnd   = BRIDGE_START * TILE_DIMENSION + halfWidth;
        float rightEnd  = (BRIDGE_START + BRIDGE_LENGTH) * TILE_DIMENSION - halfWidth;

        Vector2 bossPosition = mBoss->getPosition();
        if (bossPosition.x < leftEnd)  bossPosition.x = leftEnd;
        if (bossPosition.x > rightEnd) bossPosition.x = rightEnd;
        mBoss->setPosition(bossPosition);

        // touching the boss costs a heart like any enemy
        if (!mGameState.player->isFlashing() && mGameState.player->isColliding(mBoss))
            mGameState.playerHit = true;
    }
    else if (mBoss->isActive() && mBoss->getPosition().y > END_GAME_THRESHOLD)
    {
        // the boss fell into the pit he is beaten, and the door opens
        mBoss->deactivate();
        mDoorClosed->deactivate();
        mDoorOpen->activate();
        PlaySound(mVictorySound);
    }

    // the boss knocks a rock loose every few seconds faster the angrier he gets
    if (mGameState.bossHealth > 0)
    {
        mRockTimer += deltaTime;

        float rockInterval = ROCK_INTERVAL -
            (BOSS_MAX_HEALTH - mGameState.bossHealth) * ROCK_INTERVAL_STEP;

        if (mRockTimer >= rockInterval)
        {
            mRockTimer = 0.0f;

            // drop the first rock that is not already falling, right below the boss
            for (int i = 0; i < ROCK_COUNT; i++)
            {
                if (mRocks[i]->isActive()) continue;

                mRocks[i]->setPosition({
                    mBoss->getPosition().x,
                    (BRIDGE_ROW + 1) * TILE_DIMENSION + mRocks[i]->getScale().y / 2.0f
                });
                mRocks[i]->setVelocity({ 0.0f, 0.0f });
                mRocks[i]->activate();
                PlaySound(mRockSound);
                break;
            }
        }
    }

    for (int i = 0; i < ROCK_COUNT; i++)
    {
        mRocks[i]->update(deltaTime, nullptr, mGameState.map, nullptr, 0);
        if (!mRocks[i]->isActive()) continue;

        // a rock that lands on stone or falls into the pit is gone
        if (mRocks[i]->isCollidingBottom() ||
            mRocks[i]->getPosition().y > END_GAME_THRESHOLD)
            mRocks[i]->deactivate();

        // a rock that hits the player costs a heart
        else if (!mGameState.player->isFlashing() && mGameState.player->isColliding(mRocks[i]))
        {
            mGameState.playerHit = true;
            mRocks[i]->deactivate();
        }
    }

    // pit fell lose heart
    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;

    if (mGameState.player->isColliding(mDoorOpen))
        mGameState.nextSceneID = WIN;
}

void LevelD::render()
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
        { 0.0f, 0.0f }, 0.0f, BOSS_ROOM_TINT
    );

    for (int i = 0; i < DECORATION_COUNT; i++) mDecorations[i]->render();

    mGameState.map->render();
    for (int i = 0; i < BLOCK_COUNT; i++) mBridge[i].render();
    mDoorClosed->render();
    mDoorOpen->render();

    for (int i = 0; i < ROCK_COUNT; i++) mRocks[i]->render();
    mBoss->render();
    for (int i = 0; i < CRACKED_COUNT; i++) mExplosions[i]->render();
    mGameState.player->render();
}

void LevelD::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    for (int i = 0; i < DECORATION_COUNT; i++) {
        delete mDecorations[i];
        mDecorations[i] = nullptr;
    }

    delete[] mBridge;
    mBridge = nullptr;

    delete mBoss;
    mBoss = nullptr;

    for (int i = 0; i < CRACKED_COUNT; i++) {
        delete mExplosions[i];
        mExplosions[i] = nullptr;
    }

    for (int i = 0; i < ROCK_COUNT; i++) {
        delete mRocks[i];
        mRocks[i] = nullptr;
    }

    delete mDoorClosed;
    mDoorClosed = nullptr;

    delete mDoorOpen;
    mDoorOpen = nullptr;

    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};

    UnloadSound(mBreakSound);
    mBreakSound = {};
    UnloadSound(mBossHitSound);
    mBossHitSound = {};
    UnloadSound(mRockSound);
    mRockSound = {};
    UnloadSound(mCollapseSound);
    mCollapseSound = {};
    UnloadSound(mVictorySound);
    mVictorySound = {};

    UnloadTexture(mGameState.background);
    mGameState.background = {};
    
    mGameState.bossHealth = 0;
}
