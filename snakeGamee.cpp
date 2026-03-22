#include <iostream>
using namespace std;
#include "Food.h"
#include "globalVariables.h"
#include "raylib.h"
#include "snake.h"
#include <deque>
#include <raymath.h>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// global variables (all declared in globalVariables.h for cross-file access)
Color green = (Color){173, 204, 96, 255};
Color darkGreen = (Color){43, 51, 24, 255};
int cellSize = 30;         // 30 pixels
int cellCount = 25;        // 25 cells on each row of grid
int offset = 75;           // width of window borders
double lastUpdateTime = 0; // last time the snake moved (0 by default)
const double moveInterval =
    0.2; // snake moves every 0.2s (used for smooth interpolation)

// prevent food spawning in snake
// body positions(deque elements)
bool eventTriggered(double interval); // interval update checker

std::string GetResourcePath(const char *fileName) {
  if (FileExists(fileName)) {
    return fileName;
  }

  const char *applicationDirectory = GetApplicationDirectory();
  if (applicationDirectory == nullptr) {
    return fileName;
  }

  std::string resolvedPath = applicationDirectory;
  if (!resolvedPath.empty() && resolvedPath.back() != '/') {
    resolvedPath += '/';
  }

  resolvedPath += fileName;
  if (FileExists(resolvedPath.c_str())) {
    return resolvedPath;
  }

  return fileName;
}

// Main Game class to manage game state
class Game {
public:
  Snake snake;
  Food food = Food(snake.body);
  int score = 0;
  Music backgroundSound = {0};
  Sound eatSound = {0};
  Sound borderCollisionSound = {0};
  bool running = true;

  Game() {
    InitAudioDevice();
    backgroundSound =
        LoadMusicStream(GetResourcePath("backGroundSong.mp3").c_str());
    eatSound = LoadSound(GetResourcePath("munch-sound-effect.mp3").c_str());
    borderCollisionSound = LoadSound(GetResourcePath("erro.mp3").c_str());

    if (IsMusicValid(backgroundSound)) {
      PlayMusicStream(backgroundSound);
      SeekMusicStream(backgroundSound, 35.5f); // skip to 35.5s
    }
  }

  ~Game() {
    if (IsMusicValid(backgroundSound)) {
      UnloadMusicStream(backgroundSound);
    }
    if (IsSoundValid(eatSound)) {
      UnloadSound(eatSound);
    }
    if (IsSoundValid(borderCollisionSound)) {
      UnloadSound(borderCollisionSound);
    }
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
      if (IsSoundValid(eatSound)) {
        PlaySound(eatSound);
      }
      food.position = food.GenerateRandomPosition(snake.body);
      snake.addSegment = true;
      score++;
    }
  }

  void checkCollisionWithEdges() {
    if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
      if (IsSoundValid(borderCollisionSound)) {
        PlaySound(borderCollisionSound);
      }
      GameOver();
    }
    if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
      if (IsSoundValid(borderCollisionSound)) {
        PlaySound(borderCollisionSound);
      }
      GameOver();
    }
  }

  void GameOver() {
    snake.reset();
    food.position = food.GenerateRandomPosition(snake.body);
    running = false;
    score = 0;
    if (IsMusicValid(backgroundSound)) {
      StopMusicStream(backgroundSound);
    }
  }

  void checkCollisionWithTail() {
    deque<Vector2> headlessBody = snake.body;
    headlessBody.pop_front(); // remove head element ([0])from body deque

    if (elementInDeque(snake.body[0], headlessBody)) {
      if (IsSoundValid(borderCollisionSound)) {
        PlaySound(borderCollisionSound);
      }
      GameOver();
    }
  }
};

// Global game pointer so UpdateDrawFrame can access it
Game *gamePtr = nullptr;

void UpdateDrawFrame(void) {
  if (IsMusicValid(gamePtr->backgroundSound)) {
    UpdateMusicStream(gamePtr->backgroundSound);
  }

  BeginDrawing();

  double timeSinceLastMove = GetTime() - lastUpdateTime;
  float t = (float)(timeSinceLastMove / moveInterval);

  // snake movement update
  if (eventTriggered(moveInterval)) {
    gamePtr->Update();
  }

  if (IsKeyPressed(KEY_UP) && gamePtr->snake.direction.y != 1) {
    gamePtr->snake.direction = (Vector2){0, -1};
    gamePtr->running = true;
    if (IsMusicValid(gamePtr->backgroundSound)) {
      ResumeMusicStream(gamePtr->backgroundSound);
    }
  }
  if (IsKeyPressed(KEY_DOWN) && gamePtr->snake.direction.y != -1) {
    gamePtr->snake.direction = (Vector2){0, 1};
    gamePtr->running = true;
  }
  if (IsKeyPressed(KEY_LEFT) && gamePtr->snake.direction.x != 1) {
    gamePtr->snake.direction = (Vector2){-1, 0};
    gamePtr->running = true;
  }
  if (IsKeyPressed(KEY_RIGHT) && gamePtr->snake.direction.x != -1) {
    gamePtr->snake.direction = (Vector2){1, 0};
    gamePtr->running = true;
  }

  ClearBackground(green);
  Rectangle rect;
  rect.x = (float)offset - 5;
  rect.y = (float)offset - 5;
  rect.width = (float)cellSize * cellCount + 10;
  rect.height = (float)cellSize * cellCount + 10;
  DrawRectangleLinesEx(rect, 5, darkGreen);
  DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
  DrawText(TextFormat("%i", gamePtr->score), offset - 5,
           offset + cellSize * cellCount + 10, 40, darkGreen);
  gamePtr->Draw();

  EndDrawing();
}

int main() {
  InitWindow(2 * offset + cellSize * cellCount, offset + cellSize * cellCount,
             "Retro Snake");

  SetTargetFPS(60);

  Game game;
  gamePtr = &game;

// Web Build: Emscripten requires a callback function to manage the game loop
// It cannot use an infinite while loop like desktop apps because it would block
// the browser UI
#ifdef __EMSCRIPTEN__
  // 60 FPS, simulate infinite loop (1)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  // Native Build: Standard infinite game loop
  // Runs until the user closes the window (ESC or Close button)
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  CloseWindow();
  return 0;
}

bool elementInDeque(Vector2 element, const deque<Vector2> &deque) {
  for (unsigned int i = 0; i < deque.size(); i++) {
    // if head position matches any apple position, return true
    if (deque[i].x == element.x && deque[i].y == element.y) {
      return true;
    }
  }
  return false;
}
bool eventTriggered(double interval) {
  double currentTime = GetTime();
  if (currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    return true;
  } else {
    return false;
  }
};
