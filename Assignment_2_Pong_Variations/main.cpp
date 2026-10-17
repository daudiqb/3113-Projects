/**

* Author: Muhammad Daud Iqbal Rana

* Assignment: Pong Clone

* Date due: [10/16/2026]

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/cs3113.h"

enum GameMode { TWO_PLAYER, SINGLE_PLAYER };
enum GameState { START, PLAYING, GAME_OVER };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr Vector2 PADDLE_SIZE = { 175.0f, 200.0f };
constexpr float PADDLE_VISIBLE_RATIO = 0.45f;
constexpr float MIN_PADDLE_HEIGHT = 140.0f,
                MAX_PADDLE_HEIGHT = 300.0f,
                RESIZE_SPEED      = 200.0f;

constexpr Vector2 P1_INIT_POS = { 50.0f, SCREEN_HEIGHT / 2 };
constexpr Vector2 P2_INIT_POS = { SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT / 2 };

constexpr float BALL_SPEED         = 250.0f,
                MAX_BALL_SPEED     = 550.0f,
                BALL_ACCELERATION  = 15.0f;

constexpr int MAX_BALLS = 3;

constexpr Vector2 BALL_SIZE = { 45.0f, 45.0f };
constexpr Vector2 BALL_INIT_POS = ORIGIN;

// Collision
constexpr Vector2 PADDLE_COLLISION_SIZE = { 25.0f, 90.0f };
constexpr Vector2 BALL_COLLISION_SIZE = { 15.0f, 15.0f };

// Speed of paddle when single plater
constexpr float AI_SPEED = 180.0f;

// Best of 5
constexpr int WINNING_SCORE = 5;

constexpr Vector2 BACKGROUND_SIZE = {
    static_cast<float>(SCREEN_WIDTH),
    static_cast<float>(SCREEN_HEIGHT)
};

constexpr char BACKGROUND_FP[] = "assets/game/background.png";
constexpr char PLAYER1_FP[]    = "assets/game/redpong.png";
constexpr char PLAYER2_FP[]    = "assets/game/bluepong.png";
constexpr char BALL_FP[] = "assets/game/balls.png";
constexpr char NUMBERS_FP[] = "assets/game/numbers.png";
constexpr char GAMEOVERBG_FP[]  = "assets/game/dramatic_overlay.png";
constexpr char GAMEOVER_FP[]    = "assets/game/game_over_title.png";
constexpr char PLAYER1WINS_FP[] = "assets/game/player1_wins.png";
constexpr char PLAYER2WINS_FP[] = "assets/game/player2_wins.png";
constexpr char RESTART_FP[]     = "assets/game/press_r_restart.png";
constexpr char START_FP[] = "assets/game/start.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
GameMode gGameMode = TWO_PLAYER;
GameState gGameState = START;

float gAngle = 0.0f,
      gPreviousTicks = 0.0f;

float gPlayer1Resize = 0.0f,
      gPlayer2Resize = 0.0f;

Vector2 gPlayer1Position = P1_INIT_POS,
        gPlayer1Movement = { 0.0f, 0.0f },
        gPlayer1Scale    = PADDLE_SIZE,

        gPlayer2Position = P2_INIT_POS,
        gPlayer2Movement = { 0.0f, 0.0f },
        gPlayer2Scale    = PADDLE_SIZE;

int gActiveBalls = 1;
int gPlayer1Score = 0;
int gPlayer2Score = 0;
int gWinner = 0;

Vector2 gBallPosition[MAX_BALLS] = {
    BALL_INIT_POS,
    BALL_INIT_POS,
    BALL_INIT_POS
};

Vector2 gBallMovement[MAX_BALLS] = {
    {  1.0f,  0.6f },
    { -1.0f, -0.7f },
    {  1.0f, -0.9f }
};

float gBallSpeed[MAX_BALLS] = {
    BALL_SPEED,
    BALL_SPEED,
    BALL_SPEED
};

Vector2 gBallScale = BALL_SIZE;

Texture2D gBackgroundTexture;
Texture2D gPlayer1Texture;
Texture2D gPlayer2Texture;
Texture2D gBallTexture;
Texture2D gNumbersTexture;
Texture2D gGameOverBGTexture;
Texture2D gGameOverTexture;
Texture2D gPlayer1WinsTexture;
Texture2D gPlayer2WinsTexture;
Texture2D gRestartTexture;
Texture2D gStartTexture;

// Function Declarations
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB);
void initialise();
void processInput();
void update();
void render();
void shutdown();
void resetGame();

void renderObject(const Texture2D *texture,
                  const Vector2 *position,
                  const Vector2 *scale);

// Function Definitions
void renderObject(const Texture2D *texture,
                  const Vector2 *position,
                  const Vector2 *scale)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        scale->x,
        scale->y
    };

    Vector2 originOffset = {
        scale->x / 2.0f,
        scale->y / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea,
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) -
                      ((scaleA->x + scaleB->x) / 2.0f);

    float yDistance = fabs(positionA->y - positionB->y) -
                      ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void resetGame()
{
    gGameState = PLAYING;
    gGameMode = TWO_PLAYER;
    gWinner = 0;

    gPlayer1Score = 0;
    gPlayer2Score = 0;
    gActiveBalls = 1;

    gPlayer1Position = P1_INIT_POS;
    gPlayer2Position = P2_INIT_POS;

    gPlayer1Scale = PADDLE_SIZE;
    gPlayer2Scale = PADDLE_SIZE;

    gPlayer1Movement = { 0.0f, 0.0f };
    gPlayer2Movement = { 0.0f, 0.0f };

    gPlayer1Resize = 0.0f;
    gPlayer2Resize = 0.0f;

    gBallMovement[0] = {  1.0f,  0.6f };
    gBallMovement[1] = { -1.0f, -0.7f };
    gBallMovement[2] = {  1.0f, -0.9f };

    for (int i = 0; i < MAX_BALLS; i++)
    {
        gBallPosition[i] = BALL_INIT_POS;
        gBallSpeed[i] = BALL_SPEED;
        Normalise(&gBallMovement[i]);
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Pong");

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gPlayer1Texture = LoadTexture(PLAYER1_FP);
    gPlayer2Texture = LoadTexture(PLAYER2_FP);
    gBallTexture = LoadTexture(BALL_FP);
    gNumbersTexture = LoadTexture(NUMBERS_FP);
    gGameOverBGTexture = LoadTexture(GAMEOVERBG_FP);
    gGameOverTexture = LoadTexture(GAMEOVER_FP);
    gPlayer1WinsTexture = LoadTexture(PLAYER1WINS_FP);
    gPlayer2WinsTexture = LoadTexture(PLAYER2WINS_FP);
    gRestartTexture = LoadTexture(RESTART_FP);
    gStartTexture = LoadTexture(START_FP);
    
    for (int i = 0; i < MAX_BALLS; i++) {
        Normalise(&gBallMovement[i]);
    }

    SetTargetFPS(FPS);
}


void processInput()
{
    // Reset movement vectors
    gPlayer1Movement = { 0.0f, 0.0f };
    gPlayer2Movement = { 0.0f, 0.0f };

    // Reset resize directions
    gPlayer1Resize = 0.0f;
    gPlayer2Resize = 0.0f;

    // Quit
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
        gAppStatus = TERMINATED; }

    // The home page to start up the game
    if (gGameState == START)
    {
        if (IsKeyPressed(KEY_ENTER))
            gGameState = PLAYING;

        return;
    }
    
    // Restarting the session when the game ends
    if (gGameState == GAME_OVER) {
        if (IsKeyPressed(KEY_R))
            resetGame();
        return; }

    // Swithcing between the two game modes
    if (IsKeyPressed(KEY_T)) {
        if (gGameMode == TWO_PLAYER) {
            gGameMode = SINGLE_PLAYER; }
        else {
            gGameMode = TWO_PLAYER; }
    }

    // Movements for the player 1
    if (IsKeyDown(KEY_W)) {
        gPlayer1Movement.y = -1.0f;
    } else if (IsKeyDown(KEY_S)) {
        gPlayer1Movement.y =  1.0f; }

    // Extending and Retracting function for player 1
    if (IsKeyDown(KEY_D)){
        gPlayer1Resize =  1.0f;
    } else if (IsKeyDown(KEY_A)){
        gPlayer1Resize = -1.0f; }

    // Making sure that player 2 only works in two player mode
    if (gGameMode == TWO_PLAYER) {

        if (IsKeyDown(KEY_UP)){
            gPlayer2Movement.y = -1.0f;
        } else if (IsKeyDown(KEY_DOWN)) {
            gPlayer2Movement.y =  1.0f; }

        if (IsKeyDown(KEY_RIGHT)) {
            gPlayer2Resize =  1.0f;
        } else if (IsKeyDown(KEY_LEFT)) {
            gPlayer2Resize = -1.0f; }
    }

    // Mumbers of balls to be spawned on screen
    if (IsKeyPressed(KEY_ONE)) {
        gActiveBalls = 1;
    }

    if (IsKeyPressed(KEY_TWO)) {
        gActiveBalls = 2; }

    if (IsKeyPressed(KEY_THREE)) {
        gActiveBalls = 3; }
}

void update()
{
    // Delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    if (gGameState != PLAYING)
        return;

    // Player 1 resize
    gPlayer1Scale.y += RESIZE_SPEED * gPlayer1Resize * deltaTime;

    // Player 2 resize
    gPlayer2Scale.y += RESIZE_SPEED * gPlayer2Resize * deltaTime;

    // Player 1 resize limits
    if (gPlayer1Scale.y > MAX_PADDLE_HEIGHT)
        gPlayer1Scale.y = MAX_PADDLE_HEIGHT;

    if (gPlayer1Scale.y < MIN_PADDLE_HEIGHT)
        gPlayer1Scale.y = MIN_PADDLE_HEIGHT;

    // Player 2 resize limits
    if (gPlayer2Scale.y > MAX_PADDLE_HEIGHT)
        gPlayer2Scale.y = MAX_PADDLE_HEIGHT;

    if (gPlayer2Scale.y < MIN_PADDLE_HEIGHT)
        gPlayer2Scale.y = MIN_PADDLE_HEIGHT;

    // Making so paddle move slower if extended and faster
    // when retracted
    float player1Speed = 320.0f -
        (gPlayer1Scale.y - MIN_PADDLE_HEIGHT) *
        (200.0f / (MAX_PADDLE_HEIGHT - MIN_PADDLE_HEIGHT));

    float player2Speed = 320.0f -
        (gPlayer2Scale.y - MIN_PADDLE_HEIGHT) *
        (200.0f / (MAX_PADDLE_HEIGHT - MIN_PADDLE_HEIGHT));


// Mode for singleplayer
if (gGameMode == SINGLE_PLAYER) {
    // Make the paddle normal size if changed by player 2
    gPlayer2Resize = 0.0f;

    // Follow the ball vertically
    if (gPlayer2Position.y < gBallPosition[0].y - 10.0f) {
        gPlayer2Movement.y = 1.0f; }
    else if (gPlayer2Position.y > gBallPosition[0].y + 10.0f) {
        gPlayer2Movement.y = -1.0f; }
    else {
        gPlayer2Movement.y = 0.0f; }
}

    // Player 1 movement
    gPlayer1Position.y += player1Speed * gPlayer1Movement.y * deltaTime;

    // Player 2 movement
    if (gGameMode == SINGLE_PLAYER) {
        gPlayer2Position.y += AI_SPEED * gPlayer2Movement.y * deltaTime; }
    else {
        gPlayer2Position.y += player2Speed * gPlayer2Movement.y * deltaTime; }

    // Visible paddle heights
    float p1HalfHeight = gPlayer1Scale.y * PADDLE_VISIBLE_RATIO / 2.0f;
    float p2HalfHeight = gPlayer2Scale.y * PADDLE_VISIBLE_RATIO / 2.0f;

    // Screen boundaries for player 1
    if (gPlayer1Position.y - p1HalfHeight < 0.0f)
        gPlayer1Position.y = p1HalfHeight;

    if (gPlayer1Position.y + p1HalfHeight > SCREEN_HEIGHT)
        gPlayer1Position.y = SCREEN_HEIGHT - p1HalfHeight;

    // PScreen boundaries for player 2
    if (gPlayer2Position.y - p2HalfHeight < 0.0f)
        gPlayer2Position.y = p2HalfHeight;

    if (gPlayer2Position.y + p2HalfHeight > SCREEN_HEIGHT)
        gPlayer2Position.y = SCREEN_HEIGHT - p2HalfHeight;

    // Wanna make sure that the collision box scale with the paddles
    // as the extend and retract
    Vector2 player1CollisionSize = {
        PADDLE_COLLISION_SIZE.x,
        gPlayer1Scale.y * PADDLE_VISIBLE_RATIO
    };

    Vector2 player2CollisionSize = {
        PADDLE_COLLISION_SIZE.x,
        gPlayer2Scale.y * PADDLE_VISIBLE_RATIO
    };

    // Update only active balls
    for (int i = 0; i < gActiveBalls; i++) {

        // to make it more fun ball speed increases over time
        if (gBallSpeed[i] < MAX_BALL_SPEED)
            gBallSpeed[i] += BALL_ACCELERATION * deltaTime;

        if (gBallSpeed[i] > MAX_BALL_SPEED)
            gBallSpeed[i] = MAX_BALL_SPEED;

        // Ball movement
        gBallPosition[i].x += gBallSpeed[i] * gBallMovement[i].x * deltaTime;

        gBallPosition[i].y += gBallSpeed[i] * gBallMovement[i].y * deltaTime;

        // Top wall
        if (gBallPosition[i].y - BALL_COLLISION_SIZE.y / 2.0f < 0.0f) {
            gBallPosition[i].y = BALL_COLLISION_SIZE.y / 2.0f;
            gBallMovement[i].y = -gBallMovement[i].y;
        }

        // Collision with the bottom wall
        if (gBallPosition[i].y + BALL_COLLISION_SIZE.y / 2.0f > SCREEN_HEIGHT) {
            gBallPosition[i].y = SCREEN_HEIGHT - BALL_COLLISION_SIZE.y / 2.0f;
            gBallMovement[i].y = -gBallMovement[i].y;
        }

        // collision with player 1 paddle
        if (gBallMovement[i].x < 0.0f &&
            gBallPosition[i].x > gPlayer1Position.x &&
            isColliding(&gBallPosition[i], &BALL_COLLISION_SIZE,
                        &gPlayer1Position, &player1CollisionSize)) {
            
            gBallMovement[i].x = -gBallMovement[i].x;

            gBallPosition[i].x = gPlayer1Position.x +
                player1CollisionSize.x / 2.0f +
                BALL_COLLISION_SIZE.x / 2.0f;
        }

        // collision with player 2 paddle
        if (gBallMovement[i].x > 0.0f &&
            gBallPosition[i].x < gPlayer2Position.x &&
            isColliding(&gBallPosition[i], &BALL_COLLISION_SIZE,
                        &gPlayer2Position, &player2CollisionSize)) {
            
            gBallMovement[i].x = -gBallMovement[i].x;

            gBallPosition[i].x = gPlayer2Position.x -
                player2CollisionSize.x / 2.0f -
                BALL_COLLISION_SIZE.x / 2.0f;
        }

        // scoring mechanism for both players
        if (gBallPosition[i].x < 0.0f) {
            gPlayer2Score++;

            gBallPosition[i] = BALL_INIT_POS;
            gBallMovement[i].x = -gBallMovement[i].x;
            gBallSpeed[i] = BALL_SPEED;
        } else if (gBallPosition[i].x > SCREEN_WIDTH) {
            gPlayer1Score++;

            gBallPosition[i] = BALL_INIT_POS;
            gBallMovement[i].x = -gBallMovement[i].x;
            gBallSpeed[i] = BALL_SPEED;
        }

        // Check winning condition
        if (gPlayer1Score >= WINNING_SCORE) {
            gWinner = 1;
            gGameState = GAME_OVER;
            break; }
        else if (gPlayer2Score >= WINNING_SCORE) {
            gWinner = 2;
            gGameState = GAME_OVER;
            break; }
    }
}


void render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Start screen
    if (gGameState == START)
    {
        renderObject(&gBackgroundTexture, &ORIGIN, &BACKGROUND_SIZE);

        // Sclaling the start screen, image was huge.
        float startScale = 0.65f;

        Vector2 startPosition = { 400.0f, 225.0f };
        Vector2 startSize = {
            gStartTexture.width * startScale,
            gStartTexture.height * startScale
        };

        renderObject(
            &gStartTexture,
            &startPosition,
            &startSize
        );

        EndDrawing();
        return;
    }

    // Background
    renderObject(&gBackgroundTexture, &ORIGIN, &BACKGROUND_SIZE);

    // Paddles
    renderObject(&gPlayer1Texture, &gPlayer1Position, &gPlayer1Scale);
    renderObject(&gPlayer2Texture, &gPlayer2Position, &gPlayer2Scale);


    // Ball texture area
    Rectangle ballTextureArea = {
        0.0f,
        0.0f,
        gBallTexture.width / 2.0f,
        gBallTexture.height / 2.0f
    };

    Vector2 ballOrigin = {
        gBallScale.x / 2.0f,
        gBallScale.y / 2.0f
    };

    // Draw only active balls
    for (int i = 0; i < gActiveBalls; i++)
    {
        Rectangle ballDestinationArea = {
            gBallPosition[i].x,
            gBallPosition[i].y,
            gBallScale.x,
            gBallScale.y
        };

        DrawTexturePro(
            gBallTexture,
            ballTextureArea,
            ballDestinationArea,
            ballOrigin,
            gAngle,
            WHITE
        );
    }
    
    // Scoreboard
    float digitWidth = gNumbersTexture.width / 6.0f;
    float digitHeight = gNumbersTexture.height / 2.0f;

    // Player 1 score which is orange, bottom row
    Rectangle player1ScoreArea = {
        gPlayer1Score * digitWidth,
        digitHeight,
        digitWidth,
        digitHeight
    };

    // Player 2 scorewhich is blue, top row
    Rectangle player2ScoreArea = {
        gPlayer2Score * digitWidth,
        0.0f,
        digitWidth,
        digitHeight
    };

    // where the scores should be displayed on screen
    Rectangle player1ScoreDestination = {
        300.0f, 55.0f, 45.0f, 55.0f
    };

    Rectangle player2ScoreDestination = {
        500.0f, 55.0f, 45.0f, 55.0f
    };

    Vector2 scoreOrigin = {
        22.5f, 27.5f
    };

    // Draw Player 1 score
    DrawTexturePro(
        gNumbersTexture,
        player1ScoreArea,
        player1ScoreDestination,
        scoreOrigin,
        0.0f,
        WHITE
    );

    // Draw Player 2 score
    DrawTexturePro(
        gNumbersTexture,
        player2ScoreArea,
        player2ScoreDestination,
        scoreOrigin,
        0.0f,
        WHITE
    );
    
    if (gGameState == GAME_OVER)
    {
        renderObject(&gGameOverBGTexture, &ORIGIN, &BACKGROUND_SIZE);

        // message display for game over, have a darker background as well
        Vector2 gameOverPosition = { 400.0f, 210.0f };
        Vector2 gameOverSize = { 400.0f, 220.0f };

        renderObject(
            &gGameOverTexture,
            &gameOverPosition,
            &gameOverSize
        );

        // message diplayer for the winner
        Vector2 winnerPosition = { 400.0f, 235.0f };
        Vector2 winnerSize = { 600.0f, 250.0f };

        if (gWinner == 1)
        {
            renderObject(
                &gPlayer1WinsTexture,
                &winnerPosition,
                &winnerSize
            );
        }
        else if (gWinner == 2)
        {
            renderObject(
                &gPlayer2WinsTexture,
                &winnerPosition,
                &winnerSize
            );
        }

        // instructions to restart the game
        Vector2 restartPosition = { 400.0f, 270.0f };
        Vector2 restartSize = { 400.0f, 200.0f };

        renderObject(
            &gRestartTexture,
            &restartPosition,
            &restartSize
        );
    }

    EndDrawing();
}


void shutdown()
{
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gPlayer1Texture);
    UnloadTexture(gPlayer2Texture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gNumbersTexture);
    UnloadTexture(gGameOverBGTexture);
    UnloadTexture(gGameOverTexture);
    UnloadTexture(gPlayer1WinsTexture);
    UnloadTexture(gPlayer2WinsTexture);
    UnloadTexture(gRestartTexture);
    UnloadTexture(gStartTexture);

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
