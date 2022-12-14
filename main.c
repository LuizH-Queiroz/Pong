#include "raylib.h"
#include <time.h>
#include <stdbool.h>


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define DELTA_TIME GetFrameTime()

#define WIN_POINTS 5

#define PADDLE_SPEED 650
#define LATERAL_DISTANCE 50 // Paddle distance from screen borders



//----------------------------------------------------------------------------------
// Sound Effects
//----------------------------------------------------------------------------------
static Sound paddle_hit;
static Sound score;
static Sound wall_hit;



//----------------------------------------------------------------------------------
// State Machine's enum and Ball Struct definition
//----------------------------------------------------------------------------------
typedef enum {
    SERVE_ENTER,
    SERVE_UPDATE,

    GAME_ENTER,
    GAME_UPDATE,

    END_ENTER,
    END_UPDATE
} STATE_MACHINE;


typedef struct {
    int x, y; // Positions on X and Y axis
    int width, height;
    float dx, dy; // Velocity on X and Y axis
} tBall;



//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

// General Purposes Functions
int min(int a, int b);
int max(int a, int b);
bool collides(tBall ball, Rectangle paddle);

void Player1_Update();
void Ball_Update();
void Player2_Update();


// Game State Functions
void Serve_Enter();
void Serve_Update();
void Serve_Draw();

void Game_Enter();
void Game_Update();
void Game_Draw();

void End_Enter();
void End_Update();
void End_Draw();



//----------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------
static STATE_MACHINE Global_StateMachine;

static Rectangle Player1;
static Rectangle Player2;
static tBall Ball;

static int Player1Points;
static int Player2Points;



//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    const Texture2D background = LoadTexture("resources/background.png");
    const Texture2D cup = LoadTexture("resources/cup.png");

    // Audio
    InitAudioDevice();
    paddle_hit = LoadSound("resources/paddle_hit.wav");
    score = LoadSound("resources/score.wav");
    SetSoundVolume(score, 0.5);
    wall_hit = LoadSound("resources/wall_hit.wav");

    // Global State Machine
    Global_StateMachine = SERVE_ENTER;

    // Player1 and Player2 paddle
    Player1.width = 18;
    Player1.height = 86;
    Player1.x = LATERAL_DISTANCE;
    Player1.y = SCREEN_HEIGHT/2 - Player1.height/2;

    Player2.width = 18;
    Player2.height = 86;
    Player2.x = SCREEN_WIDTH - (LATERAL_DISTANCE + Player2.width);
    Player2.y = SCREEN_HEIGHT/2 - Player2.height/2;

    // Ball
    Ball.width = Ball.height = 15;

    // Points
    Player1Points = Player2Points = 0;


    //--------------------------------------------------------------------------------------

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    SetRandomSeed(time(NULL)); // Ensure random numbers when calling GetRandomValue()

    //--------------------------------------------------------------------------------------


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update variables
        switch(Global_StateMachine)
        {
            case SERVE_ENTER:
                Serve_Enter();
                break;
            case SERVE_UPDATE:
                Serve_Update();
                break;

            case GAME_ENTER:
                Game_Enter();
                break;
            case GAME_UPDATE:
                Game_Update();
                break;

            case END_ENTER:
                End_Enter();
                break;
            case END_UPDATE:
                End_Update();
                break;
        }


        // Draw in screen
        BeginDrawing();

        DrawTextureEx(background, (Vector2){0, 0}, 0, 1.26, WHITE);
        DrawTextureEx(cup, (Vector2){SCREEN_WIDTH/2-(37.5), SCREEN_HEIGHT-140}, 0, 0.25, WHITE);
        switch(Global_StateMachine)
        {
            case SERVE_ENTER:
            case SERVE_UPDATE:
                Serve_Draw();
                break;

            case GAME_ENTER:
            case GAME_UPDATE:
                Game_Draw();
                break;

            case END_ENTER:
            case END_UPDATE:
                End_Draw();
                break;

            default:
                break;
        }

        EndDrawing();
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}




//----------------------------------------------------------------------------------
// Serve State Functions
//----------------------------------------------------------------------------------
int min(int a, int b) {
    return (a < b ? a : b);
}


int max(int a, int b) {
    return (a > b ? a : b);
}


bool collides(tBall ball, Rectangle paddle) {

    // Check if it is possible to have collision based on X axis
    if (ball.x > (paddle.x + paddle.width) || paddle.x > (ball.x + ball.width))
    {
        return false;
    }

    // Check if it is possible to have collision based on Y axis
    if (ball.y > (paddle.y + paddle.height) || paddle.y > (ball.y + ball.height))
    {
        return false;
    }

    return true;
}


void Player1_Update() {
    
    if (IsKeyDown(KEY_W)) {
        Player1.y -= PADDLE_SPEED * DELTA_TIME;
        Player1.y = max(Player1.y, 0);
    }
    else if (IsKeyDown(KEY_S)) {
        Player1.y += PADDLE_SPEED * DELTA_TIME;
        Player1.y = min(Player1.y, SCREEN_HEIGHT - Player1.height);
    }

}


void Ball_Update() {
    
    // Update ball position
    Ball.x += Ball.dx * DELTA_TIME;
    Ball.y += Ball.dy * DELTA_TIME;

    // Check collision with top and bottom walls
    if (Ball.y <= 0 || Ball.y >= SCREEN_HEIGHT - Ball.height) {
        PlaySound(wall_hit);
        Ball.dy *= -1.03;
    }

    // Check collision with player 1 paddle
    if (collides(Ball, Player1)) {
        PlaySound(paddle_hit);
        Ball.dx *= -1.1;
        Ball.x = Player1.x + Player1.width;
    }

    // Check collision with player 2 paddle
    if (collides(Ball, Player2)) {
        PlaySound(paddle_hit);
        Ball.dx *= -1.1;
        Ball.x = Player2.x - Ball.width;
    }
             
}


void Player2_Update() {
    
    if (IsKeyDown(KEY_UP)) {
        Player2.y -= PADDLE_SPEED * DELTA_TIME;
        Player2.y = max(Player2.y, 0);
    }
    else if (IsKeyDown(KEY_DOWN)) {
        Player2.y += PADDLE_SPEED * DELTA_TIME;
        Player2.y = min(Player2.y, SCREEN_HEIGHT - Player2.height);
    }

}



//----------------------------------------------------------------------------------
// Serve State Functions
//----------------------------------------------------------------------------------
void Serve_Enter() {

    Ball.x = SCREEN_WIDTH/2 - Ball.width/2;
    Ball.y = SCREEN_HEIGHT/2 - Ball.height/2;

    // Change State to Update
    Global_StateMachine = SERVE_UPDATE;
}


void Serve_Update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        Global_StateMachine = GAME_ENTER;
    }

    Player1_Update();
    Player2_Update();
}


void Serve_Draw() {
    const int font_size =  MeasureText("Press ENTER to start", 40);
    DrawText("Press ENTER to start", SCREEN_WIDTH/2-(font_size/2), 150, 40, WHITE);

    DrawRectangle(Player1.x, Player1.y, Player1.width, Player1.height, WHITE); // Player Paddle
    DrawRectangle(Player2.x, Player2.y, Player2.width, Player2.height, WHITE); // AI Paddle
    DrawRectangle(Ball.x, Ball.y, Ball.width, Ball.height, WHITE); // Ball
}




//----------------------------------------------------------------------------------
// Game State Functions
//----------------------------------------------------------------------------------
void Game_Enter() {
    
    Ball.dx = GetRandomValue(400, 500) * (GetRandomValue(0, 1) == 0 ? 1 : -1);
    Ball.dy = GetRandomValue(400, 500) * (GetRandomValue(0, 1) == 0 ? 1 : -1);
    

    // Change State to Update
    Global_StateMachine = GAME_UPDATE;
}


void Game_Update() {
    
    Player1_Update();
    Player2_Update();
    Ball_Update();

    if (Ball.x < 0)
    {
        PlaySound(score);
        Player2Points++;
        if (Player2Points == WIN_POINTS)
        {
            Global_StateMachine = END_ENTER;
        }
        else
        {
            Global_StateMachine = SERVE_ENTER;
        }
    }
    else if (Ball.x > SCREEN_WIDTH)
    {
        PlaySound(score);
        Player1Points++;
        if (Player1Points == WIN_POINTS)
        {
            Global_StateMachine = END_ENTER;
        }
        else
        {
            Global_StateMachine = SERVE_ENTER;
        }
    }
}


void Game_Draw() {
    const int font_width = MeasureText(TextFormat("%i", Player1Points), 60);

    DrawRectangle(Player1.x, Player1.y, Player1.width, Player1.height, WHITE); // Player 1 Paddle
    DrawRectangle(Player2.x, Player2.y, Player2.width, Player2.height, WHITE); // PLayer 2 Paddle
    DrawRectangle(Ball.x, Ball.y, Ball.width, Ball.height, WHITE); // Ball
    DrawText(TextFormat("%i", Player1Points), SCREEN_WIDTH/2-(font_width+40), 150, 60, WHITE);
    DrawText(TextFormat("%i", Player2Points), SCREEN_WIDTH/2+(40), 150, 60, WHITE);

}




//----------------------------------------------------------------------------------
// End State Functions
//----------------------------------------------------------------------------------
void End_Enter() {


    // Change State to Update
    Global_StateMachine = END_UPDATE;
}


void End_Update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        Player1Points = 0;
        Player2Points = 0;
        Global_StateMachine = SERVE_ENTER;
    }
}


void End_Draw() {
    const int text_width = MeasureText(TextFormat("Player %i wins!", Player1Points > Player2Points ? 1 : 2), 60);
    const int text_size =  MeasureText("Press ENTER to restart", 40);
    
    DrawText(TextFormat("Player %i Wins!", (Player1Points > Player2Points ? 1 : 2)), SCREEN_WIDTH/2-(text_width/2), 150, 60, WHITE);
    DrawText(TextFormat("Press ENTER to play again"), SCREEN_WIDTH/2-(text_size/2), 250, 40, WHITE);
}