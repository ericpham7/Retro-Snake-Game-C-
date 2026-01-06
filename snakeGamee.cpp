#include <iostream>
using namespace std;
#include "raylib.h"
#include <raymath.h>
#include <deque>
#include "snake.h"
#include "Food.h"

// global variables (all declared in globalVariables.h for cross-file access)
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
int cellSize = 30; // 30 pixels
int cellCount = 25; // 25 cells on each row of grid  
int offset = 75; // width of window borders
double lastUpdateTime = 0; // last time the snake moved (0 by default)
const double moveInterval = 0.2; // snake moves every 0.2s (used for smooth interpolation)

bool elementInDeque(Vector2 element, deque<Vector2> deque); // prevent food spawning in snake body positions(deque elements)
bool eventTriggered(double interval); // interval update checker


// Main Game class to manage game state
class Game {
public:
    Snake snake;
    Food food = Food(snake.body);
    int score = 0;
    Music backgroundSound;
    Sound eatSound;
    Sound borderCollisionSound;
    bool running;

    Game() {
        running = true;
        InitAudioDevice();
        backgroundSound = LoadMusicStream("Mo'Jitos Mo'Problems.mp3");
        eatSound = LoadSound("mario.mp3");
        borderCollisionSound = LoadSound("erro.mp3");
        SetMusicVolume(backgroundSound, 0.5f);
        SetMusicPitch(backgroundSound, 1.05f);
        PlayMusicStream(backgroundSound);
    }

    ~Game() {
        UnloadMusicStream(backgroundSound);
        UnloadSound(eatSound);
        UnloadSound(borderCollisionSound);
        CloseAudioDevice();
    }
    
    void Draw() {
        snake.Draw();
        food.Draw(snake.body);
    }
    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            checkCollisionWithEdges();
            checkCollisionWithTail();
        }
    }
   
    void CheckCollisionWithFood() {
        if (snake.body[0] == food.position) {
            PlaySound(eatSound);
            food.position = food.GenerateRandomCell(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void checkCollisionWithEdges() {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
            PlaySound(borderCollisionSound);
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
            PlaySound(borderCollisionSound);
            GameOver();
        }
    }

    void GameOver() {
        snake.reset();
        food.position = food.GenerateRandomCell(snake.body);
        running = false;
        score = 0;
        StopMusicStream(backgroundSound);
    }

    void checkCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front(); // remove head element ([0])from body deque
        
        if (elementInDeque(snake.body[0], headlessBody)) {
            PlaySound(borderCollisionSound);
            GameOver();
        }
    }
};


int main () {
   
    // InitWindow(width, height, title)
    InitWindow( 2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Retro Snake" );
  
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    
    Game game;
    
    // returns true if esc is pressed or exit button
    while (WindowShouldClose() == false) {
        UpdateMusicStream(game.backgroundSound); // update music stream buffers (always required)
        //Prepare to render a new frame
        BeginDrawing();
        // draw game objects
        game.Draw();
        // snake movement update
        if (eventTriggered(moveInterval)) {
            game.Update();
        }
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
            ResumeMusicStream(game.backgroundSound);
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = {1, 0};
            game.running = true;

        }
        
        ClearBackground(green);// clear the background to green color
        // draw game border 
        DrawRectangleLinesEx(Rectangle {(float)offset-5, (float)offset-5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5,  darkGreen );
        // draw game title text
        DrawText("Retro Snake", offset -5, 20, 40, darkGreen);
        // draw score text
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        // draw game objects (Snake and Apple)

        // ends the current frames drawing phase
        EndDrawing();
        
    }

    
    CloseWindow();
    
    return 0;
}

bool elementInDeque(Vector2 foodElement, deque<Vector2> deque) {
    for (int i = 0; i < deque.size(); i++) {
        // if head position matches any apple position, return true
        if (deque[i] == foodElement) {
            return true;
        }
        else {
            return false;
        }
    }
}
bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    else {
        return false;
    }
};

