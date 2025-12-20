#include <iostream>
using namespace std;
#include "raylib.h"
#include <deque>
#include "raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30; // 30 pixels
int cellCount = 25; // 25 cells on each row of grid
int offset = 75; // width of window borders

double lastUpdateTime = 0;

// to check if food object respawns too close to snake
bool elementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (deque[i] == element) {
            return true;
        }
    }
    return false;
};

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
};

class Snake {
public:
    deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
    Vector2 direction = {1, 0};
    bool addSegment = false;
    
    void Draw() {
        for (int i = 0; i < body.size(); i++) {
            Rectangle segment = Rectangle( offset + body[i].x * cellSize, offset + body[i].y*cellSize, (float)cellSize, (float)cellSize );
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() {
        if (addSegment == true) {
            body.push_front(body[0] + direction);
            addSegment = false;
        }
        else {
            body.pop_back(); // remove tail of snake
            body.push_front(Vector2Add(body[0], direction)); // add head
        }
    }

    // reset snakes body to original spawn position
    void reset() {
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = {1,0};
    }
};

// Food Objects
class Food {
public:
    // {x,y}
    Vector2 position;
    Texture2D texture;
    
    // Food Object Constructor
    Food(deque<Vector2> snakeBody) {
        Image image = LoadImage("greenapple.png");
        ImageResize(&image, cellSize, cellSize);  // Resize food image to 30x30
        texture = LoadTextureFromImage(image); //
        UnloadImage(image); // unload image to free memory
        position = GenerateRandomPosition(snakeBody);
    }
    // Texture destructor
    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);
        return Vector2{x, y};
    }
    
    // Position Randomizer
    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();

        while(elementInDeque(position, snakeBody)) {
        position = GenerateRandomCell();
        }
        
        return position;
    }
 
};

class Game {
public:
    Snake snake;
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Music backgroundSound;
    Sound eatSound;

    Game() {
        InitAudioDevice();
        backgroundSound = LoadMusicStream("Mo'Jitos Mo'Problems.mp3");
        eatSound = LoadSound("mario.mp3");
        PlayMusicStream(backgroundSound);
    }

    ~Game() {
        UnloadMusicStream(backgroundSound);
        CloseAudioDevice();
    }
    
    void Draw() {
        food.Draw();
        snake.Draw();
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
            food.position = food.GenerateRandomPosition(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void checkCollisionWithEdges() {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }

    void GameOver() {
        snake.reset();
        food.GenerateRandomPosition(snake.body);
        running = false;
        score = 0;
        StopMusicStream(backgroundSound);
    }

    void checkCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (elementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }
};


int main () {

    cout << "Starting the game window...\n";
    
    // InitWindow(width, height, title)
    InitWindow( 2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Retro Snake" );
  
    SetTargetFPS(60);

    Game game;
    
    // WindowShouldClose returns true if esc is pressed or exit button
    while (WindowShouldClose() == false) {
        UpdateMusicStream(game.backgroundSound);
        //Prepare to render a new frame
        BeginDrawing();
        
        
        // snake movement update
        if (eventTriggered(0.2)) {
            game.Update();
        }
        
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
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
        
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle {(float)offset-5, (float)offset-5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5,  darkGreen );
        DrawText("Retro Snake", offset -5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing(); // ends the current frames drawing phase
        
    }
        
    CloseWindow();
    
    return 0;
    // const int SCREEN_WIDTH = 800;
    // const int SCREEN_HEIGHT = 600;
    // int ball_x = 100;
    // int ball_y = 100;
    // int ball_speed_x = 5;
    // int ball_speed_y = 5;
    // int ball_radius = 15;
    //
    // cout << "Hello World" << endl;
    //
    // InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
    // SetTargetFPS(60);
    //
    // while (WindowShouldClose() == false){
    //
    //     ball_x += ball_speed_x;
    //     ball_y += ball_speed_y;
    //
    //     if(ball_x + ball_radius >= SCREEN_WIDTH || ball_x - ball_radius <= 0)
    //     {
    //         ball_speed_x *= -1;
    //     }
    //
    //     if(ball_y + ball_radius >= SCREEN_HEIGHT || ball_y - ball_radius <= 0)
    //     {
    //         ball_speed_y *= -1;
    //     }
    //     
    //     BeginDrawing();
    //     ClearBackground(BLACK);
    //     DrawCircle(ball_x,ball_y,ball_radius, WHITE);
    //     EndDrawing();
    // }
    //
    // CloseWindow();
}