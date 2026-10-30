/**

* Author: Daud Iqbal

* Assignment: Lunar Lander!

* Date due: 10/30/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr int FLAME_FRAMES = 6;
constexpr int DRONE_FRAMES = 16;
constexpr int EXPLOSION_FRAMES = 8;

constexpr int DRONE_Y_OFFSETS[DRONE_FRAMES] = {
     0, -1, -1, -2, -2,  0,  1,  2,
     3,  3,  2,  2,  1,  0,  0,  0
};

constexpr float TILE_DIMENSION = 40.0f;

constexpr int NUMBER_OF_START_TILES = 5,
              NUMBER_OF_EXIT_TILES = 3;

constexpr int NUMBER_OF_STATIC_TILES = NUMBER_OF_START_TILES + NUMBER_OF_EXIT_TILES,
              NUMBER_OF_MOVING_TILES = 4;
              
constexpr int NUMBER_OF_ACID_TILES = SCREEN_WIDTH / static_cast<int>(TILE_DIMENSION),
              NUMBER_OF_SPIKES = 1;
              
constexpr int NUMBER_OF_LASERS = 2,
              NUMBER_OF_LASER_ENDS = 4;

enum GameState { START, PLAYING, CRASHING, MISSION_FAILED, MISSION_ACCOMPLISHED };

constexpr char BG_COLOUR[] = "#0B1020";

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr Vector2 PLAYER_INIT_POS = { 110.0f, 410.0f };
constexpr Vector2 PLAYER_SIZE = { 64.0f, 64.0f };

constexpr Vector2 FUEL_BAR_POSITION = { 20.0f, 20.0f };

constexpr Vector2 FLAME_SIZE = { 20.0f, 36.0f };

constexpr Vector2 START_PLATFORM_POS = { 150.0f, 480.0f };
constexpr Vector2 PLATFORM_INIT_POS  = { 650.0f, 400.0f };

constexpr Vector2 EXIT_POSITION = { 920.0f, 372.0f };
constexpr Vector2 EXIT_SIZE = { 64.0f, 96.0f };
constexpr Vector2 EXIT_PLATFORM_POS = { 920.0f, 440.0f };

constexpr Vector2 SWITCH_POSITION = { 920.0f, 314.0f };
constexpr Vector2 SWITCH_SIZE = { 32.0f, 32.0f };

constexpr Vector2 LASER_ONE_POS = { 350.0f, 210.0f };
constexpr Vector2 LASER_TWO_POS = { 785.0f, 400.0f };

constexpr Vector2 EXPLOSION_SIZE = { 96.0f, 96.0f };

constexpr Vector2 SPIKES_INIT_POS = {
    START_PLATFORM_POS.x + 2.0f * TILE_DIMENSION,
    START_PLATFORM_POS.y - TILE_DIMENSION
};

constexpr float ACCELERATION_OF_GRAVITY = 400.0f,
                HORIZONTAL_THRUST = 100.0f,
                VERTICAL_THRUST = 550.0f,
                FIXED_TIMESTEP = 1.0f / 60.0f; 

constexpr float MAX_FUEL = 100.0f,
                FUEL_CONSUMPTION = 4.0f,
                FUEL_BAR_MARGIN = 4.0f;

constexpr float PLATFORM_MOVE_RANGE = 75.0f,
                PLATFORM_MOVE_SPEED = 0.65f;

constexpr float SAFE_LANDING_SPEED    = 110.0f,
                SAFE_HORIZONTAL_SPEED = 100.0f,
                LANDING_TOLERANCE     = 8.0f;

constexpr float FLAME_OFFSET_X     = 20.0f,
                FLAME_OFFSET_Y     = 28.0f,
                FLAME_FRAME_SPEED  = 12.0f,
                DRONE_FRAME_HEIGHT = 32.0f;

constexpr float LASER_ONE_LENGTH = 260.0f,
                LASER_TWO_LENGTH = 240.0f,
                LASER_THICKNESS  = 12.0f;

constexpr float EXPLOSION_FRAME_SPEED = 12.0f;

constexpr char PLAYER_FP[] = "assets/game/drone.png";
constexpr char FUEL_GAUGE_FP[] = "assets/game/fuel_gauge.png";
constexpr char PLATFORM_LEFT_FP[] = "assets/game/platform_left.png";
constexpr char PLATFORM_MID_FP[] = "assets/game/platform_mid.png";
constexpr char PLATFORM_RIGHT_FP[] = "assets/game/platform_right.png";
constexpr char ACID_FP[] = "assets/game/acid_top.png";
constexpr char SPIKES_FP[] = "assets/game/spikes.png";
constexpr char FLAME_FP[] = "assets/game/flame.png";
constexpr char LASER_FP[] = "assets/game/laser.png";
constexpr char LASER_EMITTER_FP[] = "assets/game/laser_emitter.png";
constexpr char LASER_RECEIVER_FP[] = "assets/game/laser_receiver.png";
constexpr char DOOR_CLOSED_FP[] = "assets/game/door_closed.png";
constexpr char DOOR_OPEN_FP[] = "assets/game/door_open.png";
constexpr char SWITCH_OFF_FP[] = "assets/game/switch_off.png";
constexpr char SWITCH_ON_FP[] = "assets/game/switch_on.png";
constexpr char GAME_OVER_FP[] = "assets/game/mission_failed.png";
constexpr char RESTART_FP[] = "assets/game/press_r_restart.png";
constexpr char SUCCESS_FP[] = "assets/game/mission_accomplished.png";
constexpr char OVERLAY_FP[] = "assets/game/overlay.png";
constexpr char EXPLOSION_FP[] = "assets/game/explosion.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
GameState gGameState = START;

int gFlameFrame = 0;
int gBurningKeys = 0;
int gExplosionFrame = 0;

float gPreviousTicks = 0.0f,
      gTimeAccumulator = 0.0f;

float gFuel = MAX_FUEL;

float gPlatformTime = 0.0f;

float gFlameAnimationTime = 0.0f;

float gExplosionAnimationTime = 0.0f;

bool gIsThrusting = false;

bool gExitUnlocked = false;

bool gOnMovingPlatform = false;

Texture2D gFuelGaugeTexture;
Texture2D gBackgroundTexture;
Texture2D gBackgroundFarTexture;
Texture2D gBackgroundBuildingsTexture;
Texture2D gBackgroundFrontTexture;
Texture2D gFlameTexture;
Texture2D gGameOverTexture;
Texture2D gRestartTexture;
Texture2D gSuccessTexture;
Texture2D gOverlayTexture;
Texture2D gStartTexture;
Texture2D gExplosionTexture;
Vector2 gExplosionPosition = { 0.0f, 0.0f };

Entity *gPlayer = nullptr;
Entity *gStaticTiles = nullptr;
Entity *gMovingTiles = nullptr;
Entity *gAcidTiles = nullptr;
Entity *gSpikes = nullptr;
Entity *gLasers = nullptr;
Entity *gLaserEnds = nullptr;
Entity *gDoorClosed = nullptr;
Entity *gDoorOpen = nullptr;
Entity *gSwitchOff = nullptr;
Entity *gSwitchOn = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void resetGame();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Drone Escape");

    // loading the background
    gBackgroundTexture = LoadTexture("assets/game/background.png");
    gBackgroundFarTexture = LoadTexture("assets/game/background_far.png");
    gBackgroundBuildingsTexture = LoadTexture("assets/game/background_buildings.png");
    gBackgroundFrontTexture = LoadTexture("assets/game/background_front.png");

    gStartTexture = LoadTexture("assets/game/start.png"); 

    gFlameTexture = LoadTexture(FLAME_FP);
    gExplosionTexture = LoadTexture(EXPLOSION_FP);

    // player
    std::map<Direction, std::vector<int>> animationAtlas = {
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7,
                   8, 9, 10, 11, 12, 13, 14, 15 } }
    };
    
    gPlayer = new Entity(
        PLAYER_INIT_POS, PLAYER_SIZE, PLAYER_FP,
        ATLAS, { 1, 16 }, animationAtlas, PLAYER
    );

    gPlayer->setColliderDimensions({
        PLAYER_SIZE.x / 2.0f,
        PLAYER_SIZE.y * 0.8f
    });

    gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    // the starting playform
    gStaticTiles = new Entity[NUMBER_OF_STATIC_TILES];

    float startLeftX = START_PLATFORM_POS.x -
        (NUMBER_OF_START_TILES * TILE_DIMENSION) / 2.0f +
        TILE_DIMENSION / 2.0f;

    for (int i = 0; i < NUMBER_OF_START_TILES; i++) {
        const char *textureFilepath = PLATFORM_MID_FP;

        if (i == 0) { textureFilepath = PLATFORM_LEFT_FP; } 
        else if (i == NUMBER_OF_START_TILES - 1) {
            textureFilepath = PLATFORM_RIGHT_FP; }

        gStaticTiles[i].setTexture(textureFilepath);
        gStaticTiles[i].setEntityType(PLATFORM);
        gStaticTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gStaticTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        gStaticTiles[i].setPosition({ startLeftX + i * TILE_DIMENSION, START_PLATFORM_POS.y });
    }

    // the platform for exit
    float exitLeftX = EXIT_PLATFORM_POS.x -
        (NUMBER_OF_EXIT_TILES * TILE_DIMENSION) / 2.0f +
        TILE_DIMENSION / 2.0f;

    for (int i = 0; i < NUMBER_OF_EXIT_TILES; i++) {
        int tileIndex = NUMBER_OF_START_TILES + i;
        const char *textureFilepath = PLATFORM_MID_FP;

        if (i == 0) { textureFilepath = PLATFORM_LEFT_FP; }
        else if (i == NUMBER_OF_EXIT_TILES - 1) {
            textureFilepath = PLATFORM_RIGHT_FP; }

        gStaticTiles[tileIndex].setTexture(textureFilepath);
        gStaticTiles[tileIndex].setEntityType(PLATFORM);
        gStaticTiles[tileIndex].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gStaticTiles[tileIndex].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        gStaticTiles[tileIndex].setPosition({ exitLeftX + i * TILE_DIMENSION, EXIT_PLATFORM_POS.y });
    }

    // platform to move 
    gMovingTiles = new Entity[NUMBER_OF_MOVING_TILES];

    float movingLeftX = PLATFORM_INIT_POS.x -
        (NUMBER_OF_MOVING_TILES * TILE_DIMENSION) / 2.0f +
        TILE_DIMENSION / 2.0f;

    for (int i = 0; i < NUMBER_OF_MOVING_TILES; i++) {
        const char *textureFilepath = PLATFORM_MID_FP;

        if (i == 0) { textureFilepath = PLATFORM_LEFT_FP; }
        else if (i == NUMBER_OF_MOVING_TILES - 1) {
            textureFilepath = PLATFORM_RIGHT_FP; }

        gMovingTiles[i].setTexture(textureFilepath);
        gMovingTiles[i].setEntityType(PLATFORM);
        gMovingTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gMovingTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        gMovingTiles[i].setPosition({ movingLeftX + i * TILE_DIMENSION, PLATFORM_INIT_POS.y });
    }

    // bottom of the page be acid
    gAcidTiles = new Entity[NUMBER_OF_ACID_TILES];

    for (int i = 0; i < NUMBER_OF_ACID_TILES; i++) {
        gAcidTiles[i].setTexture(ACID_FP);
        gAcidTiles[i].setEntityType(BLOCK);
        gAcidTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gAcidTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
        gAcidTiles[i].setPosition({
            TILE_DIMENSION / 2.0f + i * TILE_DIMENSION,
            SCREEN_HEIGHT - TILE_DIMENSION / 2.0f });
    }

    // a single spike at the staet platform
    gSpikes = new Entity[NUMBER_OF_SPIKES];

    for (int i = 0; i < NUMBER_OF_SPIKES; i++) {
        gSpikes[i].setTexture(SPIKES_FP);
        gSpikes[i].setEntityType(BLOCK);
        gSpikes[i].setScale({ TILE_DIMENSION, TILE_DIMENSION });
        gSpikes[i].setColliderDimensions({ TILE_DIMENSION * 0.8f, TILE_DIMENSION * 0.7f });
        gSpikes[i].setPosition({ SPIKES_INIT_POS.x + i * TILE_DIMENSION, SPIKES_INIT_POS.y });
    }

    // adding laser gates, in the middle
    gLasers = new Entity[NUMBER_OF_LASERS];
    gLaserEnds = new Entity[NUMBER_OF_LASER_ENDS];

    gLasers[0].setTexture(LASER_FP);
    gLasers[0].setEntityType(BLOCK);
    gLasers[0].setPosition(LASER_ONE_POS);
    gLasers[0].setScale({ LASER_ONE_LENGTH, LASER_THICKNESS });
    gLasers[0].setColliderDimensions({ LASER_THICKNESS, LASER_ONE_LENGTH });
    gLasers[0].setAngle(90.0f);

    gLasers[1].setTexture(LASER_FP);
    gLasers[1].setEntityType(BLOCK);
    gLasers[1].setPosition(LASER_TWO_POS);
    gLasers[1].setScale({ LASER_TWO_LENGTH, LASER_THICKNESS });
    gLasers[1].setColliderDimensions({ LASER_THICKNESS, LASER_TWO_LENGTH });
    gLasers[1].setAngle(90.0f);

    for (int i = 0; i < NUMBER_OF_LASER_ENDS; i++) {
        if (i % 2 == 0) { gLaserEnds[i].setTexture(LASER_EMITTER_FP);}
        else { gLaserEnds[i].setTexture(LASER_RECEIVER_FP); }

        gLaserEnds[i].setScale({ 28.0f, 28.0f });
        gLaserEnds[i].setEntityType(NONE);
        gLaserEnds[i].setAngle(90.0f);
    }

    gLaserEnds[0].setPosition({ LASER_ONE_POS.x, LASER_ONE_POS.y - LASER_ONE_LENGTH / 2.0f });
    gLaserEnds[1].setPosition({ LASER_ONE_POS.x, LASER_ONE_POS.y + LASER_ONE_LENGTH / 2.0f });
    gLaserEnds[2].setPosition({ LASER_TWO_POS.x, LASER_TWO_POS.y - LASER_TWO_LENGTH / 2.0f });
    gLaserEnds[3].setPosition({ LASER_TWO_POS.x, LASER_TWO_POS.y + LASER_TWO_LENGTH / 2.0f });

    // a switch for the exit door, land first open door
    gDoorClosed = new Entity(EXIT_POSITION, EXIT_SIZE, DOOR_CLOSED_FP, NONE);
    gDoorOpen = new Entity(EXIT_POSITION, EXIT_SIZE, DOOR_OPEN_FP, NONE);
    gDoorOpen->deactivate();
    gDoorClosed->setColliderDimensions(EXIT_SIZE);
    gDoorOpen->setColliderDimensions(EXIT_SIZE);

    gSwitchOff = new Entity(SWITCH_POSITION, SWITCH_SIZE, SWITCH_OFF_FP, NONE);
    gSwitchOn  = new Entity(SWITCH_POSITION, SWITCH_SIZE, SWITCH_ON_FP, NONE);
    gSwitchOn->deactivate();

    // load textures
    gFuelGaugeTexture = LoadTexture(FUEL_GAUGE_FP);
    gFlameTexture = LoadTexture(FLAME_FP);
    gGameOverTexture = LoadTexture(GAME_OVER_FP);
    gRestartTexture = LoadTexture(RESTART_FP);
    gSuccessTexture = LoadTexture(SUCCESS_FP);
    gOverlayTexture = LoadTexture(OVERLAY_FP);

    gPreviousTicks = static_cast<float>(GetTime());
    SetTargetFPS(FPS);
}

void resetGame()
{
    // restting the game when the user press r
    gPlayer->setPosition(PLAYER_INIT_POS);
    gPlayer->setVelocity({ 0.0f, 0.0f });
    gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    gFuel = MAX_FUEL;
    gBurningKeys = 0;
    gIsThrusting = false;
    gFlameFrame = 0;
    gFlameAnimationTime = 0.0f;
    gPlatformTime = 0.0f;
    gOnMovingPlatform = false;
    gExplosionFrame = 0;
    gExplosionAnimationTime = 0.0f;
    gExplosionPosition = PLAYER_INIT_POS;

    float movingLeftX = PLATFORM_INIT_POS.x -
        (NUMBER_OF_MOVING_TILES * TILE_DIMENSION) / 2.0f +
        TILE_DIMENSION / 2.0f;

    for (int i = 0; i < NUMBER_OF_MOVING_TILES; i++) {
        gMovingTiles[i].setPosition({ movingLeftX + i * TILE_DIMENSION, PLATFORM_INIT_POS.y });
    }

    gExitUnlocked = false;
    gDoorClosed->activate();
    gDoorOpen->deactivate();
    gSwitchOff->activate();
    gSwitchOn->deactivate();

    gTimeAccumulator = 0.0f;
    gPreviousTicks = static_cast<float>(GetTime());
    gGameState = PLAYING;
}

void processInput()
{
    // start screen
    if (gGameState == START)
    {
        if (IsKeyPressed(KEY_ENTER))
            resetGame();

        if (IsKeyPressed(KEY_Q) || WindowShouldClose())
            gAppStatus = TERMINATED;

        return;
    }

    // have a state so it shows drone exploding before fail screen
    if (gGameState == CRASHING)
    {
        if (IsKeyPressed(KEY_Q) || WindowShouldClose())
            gAppStatus = TERMINATED;

        return;
    }
    
    // after playing game
    if (gGameState == MISSION_FAILED || gGameState == MISSION_ACCOMPLISHED) {

        if (IsKeyPressed(KEY_R)) { resetGame(); }

        if (IsKeyPressed(KEY_Q) || WindowShouldClose()) { gAppStatus = TERMINATED; }

        return;
    }

    // added acceleration
    Vector2 acceleration = { 0.0f, ACCELERATION_OF_GRAVITY };
    gBurningKeys = 0;

    if (gFuel > 0.0f) {
        if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)){
            acceleration.x = -HORIZONTAL_THRUST;
            gBurningKeys++;
        } else if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
            acceleration.x = HORIZONTAL_THRUST;
            gBurningKeys++;
        }

        if (IsKeyDown(KEY_W)) {
            acceleration.y -= VERTICAL_THRUST;
            gBurningKeys++;
        }
    }

    gIsThrusting = gBurningKeys > 0;
    gPlayer->setAcceleration(acceleration);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
    // delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    
    // drone explosion anmate
    if (gGameState == CRASHING)
    {
        gExplosionAnimationTime += deltaTime;

        if (gExplosionAnimationTime >=
            1.0f / EXPLOSION_FRAME_SPEED)
        {
            gExplosionAnimationTime = 0.0f;
            gExplosionFrame++;

            if (gExplosionFrame >= EXPLOSION_FRAMES)
                gGameState = MISSION_FAILED;
        }

        return;
    }

    if (gGameState != PLAYING) { return; }
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP) {
        gTimeAccumulator = deltaTime;
        return;
    }

    // the middle platform moving
    float previousPlatformX = gMovingTiles[0].getPosition().x;
    gPlatformTime += FIXED_TIMESTEP;

    float movingLeftX = PLATFORM_INIT_POS.x -
        (NUMBER_OF_MOVING_TILES * TILE_DIMENSION) / 2.0f + TILE_DIMENSION / 2.0f;

    float platformOffset = sinf(gPlatformTime * PLATFORM_MOVE_SPEED) * PLATFORM_MOVE_RANGE;

    for (int i = 0; i < NUMBER_OF_MOVING_TILES; i++) {
        gMovingTiles[i].setPosition({
            movingLeftX + i * TILE_DIMENSION + platformOffset,
            PLATFORM_INIT_POS.y });
    }

    float platformMovementX = gMovingTiles[0].getPosition().x - previousPlatformX;
    float platformVelocityX = platformMovementX / FIXED_TIMESTEP;

    bool wasOnMovingPlatform = gOnMovingPlatform;
    gOnMovingPlatform = false;

    if (wasOnMovingPlatform) {
        Vector2 position = gPlayer->getPosition();
        position.x += platformMovementX;
        gPlayer->setPosition(position);
    }

    float previousBottom = gPlayer->getPosition().y + gPlayer->getColliderDimensions().y / 2.0f;

    // player upate
    gPlayer->update(
        FIXED_TIMESTEP,
        gStaticTiles, NUMBER_OF_STATIC_TILES,
        nullptr, 0
    );

    Vector2 playerPosition = gPlayer->getPosition();
    Vector2 playerSize = gPlayer->getColliderDimensions();
    Vector2 playerVelocity = gPlayer->getVelocity();

    float playerLeft = playerPosition.x - playerSize.x / 2.0f;
    float playerRight = playerPosition.x + playerSize.x / 2.0f;
    float playerTop = playerPosition.y - playerSize.y / 2.0f;
    float playerBottom = playerPosition.y + playerSize.y / 2.0f;

    float platformLeft = gMovingTiles[0].getPosition().x - TILE_DIMENSION / 2.0f;
    float platformRight = gMovingTiles[NUMBER_OF_MOVING_TILES - 1].getPosition().x + TILE_DIMENSION / 2.0f;
    float platformTop = PLATFORM_INIT_POS.y - TILE_DIMENSION / 2.0f;
    float platformBottom = PLATFORM_INIT_POS.y + TILE_DIMENSION / 2.0f;

    bool horizontalOverlap = playerRight > platformLeft && playerLeft < platformRight;
    bool verticalOverlap = playerBottom >= platformTop && playerTop < platformBottom;

    if (horizontalOverlap && verticalOverlap) {
        float relativeSpeed = fabs(playerVelocity.x - platformVelocityX);

        if (wasOnMovingPlatform ||
            (previousBottom <= platformTop + LANDING_TOLERANCE &&
             playerVelocity.y >= 0.0f &&
             playerVelocity.y <= SAFE_LANDING_SPEED &&
             relativeSpeed <= SAFE_HORIZONTAL_SPEED)) {

            // make the drone not slip off platform
            playerPosition.y = platformTop - playerSize.y / 2.0f;
            gPlayer->setPosition(playerPosition);

            playerVelocity.y = 0.0f;
            gPlayer->setVelocity(playerVelocity);

            gOnMovingPlatform = true;
            gExitUnlocked = true; }
        else { gGameState = MISSION_FAILED; }
    }

    // unlocking the victory gate
    if (gExitUnlocked)
    {
        gDoorClosed->deactivate();
        gDoorOpen->activate();
        gSwitchOff->deactivate();
        gSwitchOn->activate();
    }

    // collisions hazard
    for (int i = 0; i < NUMBER_OF_ACID_TILES; i++) {
        if (gPlayer->isCollidingWith(&gAcidTiles[i])) {
            gGameState = MISSION_FAILED; }}

    for (int i = 0; i < NUMBER_OF_SPIKES; i++) {
        if (gPlayer->isCollidingWith(&gSpikes[i])) {
            gGameState = MISSION_FAILED; }}

    for (int i = 0; i < NUMBER_OF_LASERS; i++) {
        if (gPlayer->isCollidingWith(&gLasers[i])) {
            gGameState = MISSION_FAILED; }}

    // adding screen bondaries 
    playerPosition = gPlayer->getPosition();
    playerSize = gPlayer->getColliderDimensions();

    if (playerPosition.x - playerSize.x / 2.0f < 0.0f ||
        playerPosition.x + playerSize.x / 2.0f > SCREEN_WIDTH ||
        playerPosition.y - playerSize.y / 2.0f < 45.0f ||
        playerPosition.y + playerSize.y / 2.0f > SCREEN_HEIGHT) 
        { gGameState = MISSION_FAILED; }

    // collision for the exit doer
    if (gGameState == PLAYING && gExitUnlocked &&
        gPlayer->isCollidingWith(gDoorOpen)) {
        gGameState = MISSION_ACCOMPLISHED; }

    // adding two lil flames on the drone
    gFlameAnimationTime += FIXED_TIMESTEP;

    if (gFlameAnimationTime >= 1.0f / FLAME_FRAME_SPEED) {
        gFlameAnimationTime = 0.0f;
        gFlameFrame++;
        gFlameFrame %= FLAME_FRAMES;
    }

    // the fuel mechanic
    if (gIsThrusting) {
        gFuel -= gBurningKeys * FUEL_CONSUMPTION * FIXED_TIMESTEP;
        if (gFuel <= 0.0f) {
            gFuel = 0.0f;
            gPlayer->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY }); }
    }

    gTimeAccumulator = deltaTime - FIXED_TIMESTEP;
    
    // start the explosion
    if (gGameState == MISSION_FAILED) {
        gExplosionPosition = gPlayer->getPosition();
        gExplosionFrame = 0;
        gExplosionAnimationTime = 0.0f;
        gGameState = CRASHING;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // load the background
    Texture2D backgrounds[] = {
        gBackgroundTexture,
        gBackgroundFarTexture,
        gBackgroundBuildingsTexture,
        gBackgroundFrontTexture
    };

    float backgroundScale = SCREEN_WIDTH / 272.0f;

    for (int i = 0; i < 4; i++) {
        Rectangle textureArea = { 
            0.0f, 0.0f,
            static_cast<float>(backgrounds[i].width),
            static_cast<float>(backgrounds[i].height) };

        Rectangle destinationArea = { 
            0.0f,
            SCREEN_HEIGHT - backgrounds[i].height * backgroundScale,
            backgrounds[i].width * backgroundScale,
            backgrounds[i].height * backgroundScale
        };

        DrawTexturePro(
            backgrounds[i], textureArea, destinationArea,
            { 0.0f, 0.0f }, 0.0f, WHITE
        );
    }

    // items to redner
    for (int i = 0; i < NUMBER_OF_ACID_TILES; i++) {
        gAcidTiles[i].render(); }

    for (int i = 0; i < NUMBER_OF_SPIKES; i++) {
        gSpikes[i].render(); }

    for (int i = 0; i < NUMBER_OF_STATIC_TILES; i++) {
        gStaticTiles[i].render(); }

    for (int i = 0; i < NUMBER_OF_MOVING_TILES; i++) {
        gMovingTiles[i].render(); }
  
    // indicate the player to land on this platform
    if (!gExitUnlocked)
    {
        float platformCenterX =
            (gMovingTiles[0].getPosition().x +
             gMovingTiles[NUMBER_OF_MOVING_TILES - 1].getPosition().x)
            / 2.0f;

        DrawText(
            "LAND",
            static_cast<int>(platformCenterX - 24.0f),
            static_cast<int>(PLATFORM_INIT_POS.y - 78.0f),
            18, WHITE
        );

        DrawText(
            "V",
            static_cast<int>(platformCenterX - 5.0f),
            static_cast<int>(PLATFORM_INIT_POS.y - 54.0f),
            18, WHITE
        );
    }

    for (int i = 0; i < NUMBER_OF_LASERS; i++) {
        gLasers[i].render(); }

    for (int i = 0; i < NUMBER_OF_LASER_ENDS; i++) {
        gLaserEnds[i].render(); }

    gDoorClosed->render();
    gDoorOpen->render();
    gSwitchOff->render();
    gSwitchOn->render();

    if (gGameState == PLAYING &&
        gFuel > 0.0f && gPlayer->getAcceleration().y < ACCELERATION_OF_GRAVITY) {
        Rectangle flameSource = getUVRectangle( 
            &gFlameTexture, gFlameFrame, 1, FLAME_FRAMES );

        Vector2 playerPosition = gPlayer->getPosition();
        int currentFrame = gPlayer->getCurrentFrameIndex();

        float animationOffset = DRONE_Y_OFFSETS[currentFrame] *
            (PLAYER_SIZE.y / DRONE_FRAME_HEIGHT);

        Vector2 flameOrigin = { FLAME_SIZE.x / 2.0f, FLAME_SIZE.y / 2.0f };

        for (int i = 0; i < 2; i++) {
            float flameX = playerPosition.x - FLAME_OFFSET_X + 2.0f;
            if (i == 1) { flameX = playerPosition.x + FLAME_OFFSET_X; }

            Rectangle flameDestination = {
                flameX,
                playerPosition.y + FLAME_OFFSET_Y + animationOffset,
                FLAME_SIZE.x, FLAME_SIZE.y
            };

            DrawTexturePro(
                gFlameTexture, flameSource, flameDestination,
                flameOrigin, 180.0f, WHITE
            );
        }
    }

    
    // drone or epxlosion
    if (gGameState != CRASHING && gGameState != MISSION_FAILED) {
        gPlayer->render(); }

    if (gGameState == CRASHING)
    {
        Rectangle explosionSource = getUVRectangle(
            &gExplosionTexture,
            gExplosionFrame,
            1, EXPLOSION_FRAMES
        );

        Rectangle explosionDestination = {
            gExplosionPosition.x,
            gExplosionPosition.y,
            EXPLOSION_SIZE.x,
            EXPLOSION_SIZE.y
        };

        DrawTexturePro(
            gExplosionTexture,
            explosionSource,
            explosionDestination,
            {
                EXPLOSION_SIZE.x / 2.0f,
                EXPLOSION_SIZE.y / 2.0f
            },
            0.0f, WHITE
        );
    }

    Rectangle fuelGaugeSource = {
        0.0f, 0.0f,
        static_cast<float>(gFuelGaugeTexture.width),
        static_cast<float>(gFuelGaugeTexture.height)
    };

    Rectangle fuelGaugeDestination = {
        FUEL_BAR_POSITION.x, FUEL_BAR_POSITION.y,
        static_cast<float>(gFuelGaugeTexture.width),
        static_cast<float>(gFuelGaugeTexture.height)
    };

    
    // fuel percentage
    int fuelPercentage = static_cast<int>(
        (gFuel / MAX_FUEL) * 100.0f
    );

    DrawText(
        TextFormat("%d%%", fuelPercentage),
        305, 20,
        16, WHITE
    );


    DrawTexturePro(
        gFuelGaugeTexture, fuelGaugeSource, fuelGaugeDestination,
        { 0.0f, 0.0f }, 0.0f, WHITE
    );

    if (gFuel > 0.0f) {
        float fuelWidth =
            (gFuelGaugeTexture.width - 2.0f * FUEL_BAR_MARGIN) * (gFuel / MAX_FUEL);

        Rectangle fuelSource = { 8.0f, 4.0f, 1.0f, 1.0f };
        Rectangle fuelDestination = {
            FUEL_BAR_POSITION.x + FUEL_BAR_MARGIN,
            FUEL_BAR_POSITION.y + FUEL_BAR_MARGIN,
            fuelWidth,
            gFuelGaugeTexture.height - 2.0f * FUEL_BAR_MARGIN
        };

        DrawTexturePro(
            gFuelGaugeTexture, fuelSource, fuelDestination,
            { 0.0f, 0.0f }, 0.0f, WHITE
        );
    }

    // the end result
    if (gGameState == MISSION_FAILED || gGameState == MISSION_ACCOMPLISHED) {
        Rectangle screenArea = {
            0.0f, 0.0f,
            static_cast<float>(SCREEN_WIDTH),
            static_cast<float>(SCREEN_HEIGHT)
        };

        Rectangle overlaySource = {
            0.0f, 0.0f,
            static_cast<float>(gOverlayTexture.width),
            static_cast<float>(gOverlayTexture.height)
        };

        DrawTexturePro(
            gOverlayTexture, overlaySource, screenArea,
            { 0.0f, 0.0f }, 0.0f, WHITE
        );

        if (gGameState == MISSION_FAILED) {
            Rectangle gameOverSource = {
                0.0f, 0.0f,
                static_cast<float>(gGameOverTexture.width),
                static_cast<float>(gGameOverTexture.height)
            };

            Rectangle gameOverDestination = {
                0.0f, 30.0f,
                static_cast<float>(SCREEN_WIDTH),
                static_cast<float>(SCREEN_HEIGHT)
            };

            DrawTexturePro(
                gGameOverTexture, gameOverSource, gameOverDestination,
                { 0.0f, 0.0f }, 0.0f, WHITE
            );
        }

        else if (gGameState == MISSION_ACCOMPLISHED) {
            Rectangle successSource = {
                0.0f, 0.0f,
                static_cast<float>(gSuccessTexture.width),
                static_cast<float>(gSuccessTexture.height)
            };
            float successWidth = 700.0f;
            float successHeight = successWidth *
                gSuccessTexture.height / gSuccessTexture.width;

            Rectangle successDestination = {
                ORIGIN.x,
                ORIGIN.y,
                successWidth,
                successHeight
            };

            DrawTexturePro(
                gSuccessTexture,
                successSource, successDestination,
                { successWidth / 2.0f, successHeight / 2.0f },
                0.0f, WHITE
            );
        }

        Rectangle restartSource = {
            0.0f, 0.0f,
            static_cast<float>(gRestartTexture.width),
            static_cast<float>(gRestartTexture.height)
        };

        Rectangle restartDestination = {
            0.0f, -30.0f,
            static_cast<float>(SCREEN_WIDTH),
            static_cast<float>(SCREEN_HEIGHT)
        };

        DrawTexturePro(
            gRestartTexture, restartSource, restartDestination,
            { 0.0f, 0.0f }, 0.0f, WHITE
        );
    }

    // start menu
    if (gGameState == START) {
        // Dark overlay
        Rectangle overlaySource = {
            0.0f, 0.0f,
            static_cast<float>(gOverlayTexture.width),
            static_cast<float>(gOverlayTexture.height)
        };

        Rectangle screenArea = {
            0.0f, 0.0f,
            static_cast<float>(SCREEN_WIDTH),
            static_cast<float>(SCREEN_HEIGHT)
        };

        DrawTexturePro(
            gOverlayTexture,
            overlaySource, screenArea,
            { 0.0f, 0.0f },
            0.0f, WHITE
        );

        // Press Enter to Begin
        Rectangle textureArea = {
            0.0f, 0.0f,
            static_cast<float>(gStartTexture.width),
            static_cast<float>(gStartTexture.height)
        };

        Rectangle destinationArea = {
            ORIGIN.x,
            ORIGIN.y,
            480.0f, 42.0f
        };

        DrawTexturePro(
            gStartTexture,
            textureArea, destinationArea,
            { 240.0f, 21.0f },
            0.0f, WHITE
        );
    }

    EndDrawing();
}

void shutdown()
{
    // unload textures
    UnloadTexture(gFuelGaugeTexture);
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gBackgroundFarTexture);
    UnloadTexture(gBackgroundBuildingsTexture);
    UnloadTexture(gBackgroundFrontTexture);
    UnloadTexture(gFlameTexture);
    UnloadTexture(gGameOverTexture);
    UnloadTexture(gRestartTexture);
    UnloadTexture(gSuccessTexture);
    UnloadTexture(gOverlayTexture);
    UnloadTexture(gStartTexture);
    UnloadTexture(gExplosionTexture);

    // deleting entities
    delete gPlayer;
    delete[] gStaticTiles;
    delete[] gMovingTiles;
    delete[] gAcidTiles;
    delete[] gSpikes;
    delete[] gLasers;
    delete[] gLaserEnds;
    delete gDoorClosed;
    delete gDoorOpen;
    delete gSwitchOff;
    delete gSwitchOn;

    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}
