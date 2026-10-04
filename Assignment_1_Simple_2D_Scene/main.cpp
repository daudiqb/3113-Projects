#include "CS3113/cs3113.h"
#include <math.h>

/**
* Author: Daud Iqbal

* Assignment: Simple 2D Scene

* Date due: 10/05/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.
**/


// Global Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2,
                SCREEN_HEIGHT = 900 / 2,
                FPS           = 60,
                SIZE          = 200;

constexpr Vector2 BASE_SIZE   = { static_cast<float>(SIZE), static_cast<float>(SIZE) };
constexpr float DRONE_BASE_SIZE = 100.0f;

constexpr char STICKMAN_FP[] = "assets/stick_man.png";
constexpr char SAW_FP[] = "assets/saw.png";
constexpr char DRONE_FP[] = "assets/drone.png";
constexpr char CITY_FP[] = "assets/city.png";
constexpr char ROOFTOP_FP[] = "assets/rooftop.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f;
Vector2   gPosition      = { 350.0f, SCREEN_HEIGHT / 2.0f };
Vector2   gScale         = BASE_SIZE;

float gSawAngle = 0.0f;
float gSawMoveTime = 0.0f;
float gDroneAngle = 0.0f;
float gDroneTime = 0.0f;
float gSaw2Angle = 0.0f;
float gSaw2MoveTime = 0.0f;
float gBackgroundTime = 0.0f;

Color gBackgroundColor;

float gPreviousTicks  = 0.0f;

Vector2 gSawPosition = { 600.0f, 300.0f };
Vector2 gSawScale = { 80.0f, 80.0f };
Vector2 gSaw2Position = { 300.0f, 350.0f };
Vector2 gSaw2Scale = { 60.0f, 60.0f };
Vector2 gDronePosition = { 300.0f, 150.0f };
Vector2 gDroneScale = { 100.0f, 100.0f };

Texture2D gStickmanTexture;
Texture2D gSawTexture;
Texture2D gDroneTexture;
Texture2D gCityTexture;
Texture2D gRooftopTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Stickman Parkour");

    gStickmanTexture = LoadTexture(STICKMAN_FP);
    gSawTexture = LoadTexture(SAW_FP);
    gDroneTexture = LoadTexture(DRONE_FP);
    gCityTexture = LoadTexture(CITY_FP);
    gRooftopTexture = LoadTexture(ROOFTOP_FP);

    gBackgroundColor = ColorFromHex("#87CEEB");

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    // Delta Time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Saw movement
    gSawAngle += 720.0f * deltaTime;

    gSawMoveTime += 2.0f * deltaTime;

    gSawPosition.x = 600.0f + 35.0f * cos(gSawMoveTime);
    gSawPosition.y = 370.0f + 25.0f * sin(gSawMoveTime);

    // Saw 2 movement
    gSaw2Angle -= 500.0f * deltaTime;
    
    gSaw2MoveTime += 1.5f * deltaTime;

    gSaw2Position.x = 200.0f + 20.0f * cos(gSaw2MoveTime);
    gSaw2Position.y = 350.0f + 15.0f * sin(gSaw2MoveTime);

    // Drone movement
    gDroneTime += 2.0f * deltaTime;

    gDronePosition.x = gPosition.x + 100.0f * cos(gDroneTime);
    gDronePosition.y = gPosition.y - 100.0f + 40.0f * sin(gDroneTime);

    gDroneScale.x = DRONE_BASE_SIZE + 15.0f * sin(gDroneTime);
    gDroneScale.y = DRONE_BASE_SIZE + 15.0f * sin(gDroneTime);

    // Stickman movement
    gPosition.x += 120.0f * deltaTime;
    gPosition.y = 240.0f - 100.0f * cos((gPosition.x - 600.0f) * 0.015f);

    if (gPosition.x > 180.0f && gPosition.x < 330.0f) {
        gAngle += 300.0f * deltaTime;
    } else if (gPosition.x > 520.0f && gPosition.x < 680.0f) {
        gAngle -= 720.0f * deltaTime;
    } else {
        gAngle = 0.0f;
    }

    if (gPosition.x > SCREEN_WIDTH + 100) {
        gPosition.x = -100.0f;
    }

    // Background Color
    gBackgroundTime += deltaTime;

    // Blue
    if (gBackgroundTime < 5.0f) {
        gBackgroundColor = ColorFromHex("#87CEEB");
    } // Orange
    else if (gBackgroundTime < 10.0f) {
    gBackgroundColor = ColorFromHex("#F4A261");
    } // Night blue
    else if (gBackgroundTime < 15.0f) {
        gBackgroundColor = ColorFromHex("#192350");
    } // Restart
    else {
        gBackgroundTime = 0.0f;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(gBackgroundColor);

    Rectangle cityTextureArea = {
        0.0f,
        0.0f,
        static_cast<float>(gCityTexture.width),
        static_cast<float>(gCityTexture.height)
    };

    Rectangle rooftopTextureArea = {
        0.0f,
        0.0f,
        static_cast<float>(gRooftopTexture.width),
        static_cast<float>(gRooftopTexture.height)
    };

    Rectangle stickmanTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gStickmanTexture.width),
        static_cast<float>(gStickmanTexture.height)
    };

    Rectangle sawTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gSawTexture.width),
        static_cast<float>(gSawTexture.height)
    };

    Rectangle droneTextureArea = {
        0.0f, 0.0f,
        static_cast<float>(gDroneTexture.width),
        static_cast<float>(gDroneTexture.height)
    };

    // Destination rectangle – centred on gPosition
        Rectangle cityDestinationArea = {
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };

    Rectangle rooftop1DestinationArea = {
        200.0f,
        385.0f,
        320.0f,
        120.0f
    };

    Rectangle rooftop2DestinationArea = {
        600.0f,
        405.0f,
        320.0f,
        120.0f
    };

    Rectangle stickmanDestinationArea = {
        gPosition.x,
        gPosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    Rectangle sawDestinationArea = {
        gSawPosition.x,
        gSawPosition.y,
        static_cast<float>(gSawScale.x),
        static_cast<float>(gSawScale.y)
    };

    Rectangle saw2DestinationArea = {
        gSaw2Position.x,
        gSaw2Position.y,
        static_cast<float>(gSaw2Scale.x),
        static_cast<float>(gSaw2Scale.y)
    };

    Rectangle droneDestinationArea = {
        gDronePosition.x,
        gDronePosition.y,
        static_cast<float>(gDroneScale.x),
        static_cast<float>(gDroneScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 cityOrigin = {
        static_cast<float>(SCREEN_WIDTH) / 2.0f,
        static_cast<float>(SCREEN_HEIGHT) / 2.0f
    };

    Vector2 rooftop1Origin = {
        160.0f,
        60.0f
    };

    Vector2 rooftop2Origin = {
        140.0f,
        70.0f
    };

    Vector2 stickmanOrigin = {
        static_cast<float>(gScale.x) / 2.0f,
        static_cast<float>(gScale.y) / 2.0f
    };

    Vector2 sawOrigin = {
        static_cast<float>(gSawScale.x) / 2.0f,
        static_cast<float>(gSawScale.y) / 2.0f
    };

    Vector2 saw2Origin = {
        static_cast<float>(gSaw2Scale.x) / 2.0f,
        static_cast<float>(gSaw2Scale.y) / 2.0f
    };

    Vector2 droneOrigin = {
        static_cast<float>(gDroneScale.x) / 2.0f,
        static_cast<float>(gDroneScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gCityTexture,
        cityTextureArea,
        cityDestinationArea,
        cityOrigin,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gRooftopTexture,
        rooftopTextureArea,
        rooftop1DestinationArea,
        rooftop1Origin,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gRooftopTexture,
        rooftopTextureArea,
        rooftop2DestinationArea,
        rooftop2Origin,
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gStickmanTexture,
        stickmanTextureArea,
        stickmanDestinationArea,
        stickmanOrigin,
        gAngle,
        WHITE
    );

    DrawTexturePro(
        gSawTexture,
        sawTextureArea,
        sawDestinationArea,
        sawOrigin,
        gSawAngle,
        WHITE
    );

    DrawTexturePro(
        gDroneTexture,
        droneTextureArea,
        droneDestinationArea,
        droneOrigin,
        gDroneAngle,
        WHITE
    );

    DrawTexturePro(
        gSawTexture,
        sawTextureArea,
        saw2DestinationArea,
        saw2Origin,
        gSaw2Angle,
        WHITE
    );

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gStickmanTexture);
    UnloadTexture(gSawTexture);
    UnloadTexture(gDroneTexture);
    UnloadTexture(gCityTexture);
    UnloadTexture(gRooftopTexture);

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