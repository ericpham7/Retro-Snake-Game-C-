#include "Food.h"
#include "globalVariables.h"
#include "raylib.h"
#include <deque>
#include <iostream>

using namespace std;

// Food Object Constructor
Food::Food(const deque<Vector2> &snakeBody) {
  texture = {0};

  Image image = LoadImage(GetResourcePath("greenapple.png").c_str());
  if (IsImageValid(image)) {
    ImageResize(&image, cellSize, cellSize); // Resize food image to 30x30 pixels
    texture = LoadTextureFromImage(image);
    UnloadImage(image); // unload image to free memory
  }

  position = GenerateRandomCell(); // set initial food position
}

// Texture destructor
Food::~Food() {
  if (IsTextureValid(texture)) {
    UnloadTexture(texture); // unload texture to free memory after food object is destroyed
  }
}

// Draw Food on Screen Method
void Food::Draw() {
  if (IsTextureValid(texture)) {
    DrawTexture(texture, offset + position.x * cellSize,offset + position.y * cellSize, WHITE);
  }
}

// Position Randomizer
Vector2 Food::GenerateRandomCell() {
  float x = GetRandomValue(0, cellCount - 1);
  float y = GetRandomValue(0, cellCount - 1);
  return (Vector2){x, y};
}

// Position Randomizer (ensures food doesn't spawn inside snake body)
Vector2 Food::GenerateRandomPosition(const deque<Vector2> &snakeBody) {
  Vector2 position = GenerateRandomCell();

  // while food position overlaps with snake body, generate a new position
  while (elementInDeque(position, snakeBody)) {
    position = GenerateRandomCell();
  }

  return position;
}
