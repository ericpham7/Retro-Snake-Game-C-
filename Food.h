#pragma once // to indicate single inclusion of this header file during each
             // compilation
#include "globalVariables.h"
#include "raylib.h"
#include <deque>
#include <iostream>

using namespace std;

// food class for green apple objects
class Food {
public:
  Vector2 position;  // carries cell Position coordinates {x,y}
  Texture2D texture; // texture variable for food image

  // Food Object Constructor
  Food(const deque<Vector2> &snakeBody);

  // Texture destructor
  ~Food();

  // Draw Food on Screen Method
  void Draw();

  // Position Randomizer
  Vector2 GenerateRandomCell();
  Vector2 GenerateRandomPosition(const deque<Vector2> &snakeBody);
};