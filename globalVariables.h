#pragma once
#include "raylib.h"
#include <deque>

// global variables (all declared here for cross-file access)
extern Color green;
extern Color darkGreen;
extern int cellSize;
extern int cellCount;
extern int offset;
extern double lastUpdateTime;
extern const double moveInterval;

bool elementInDeque(Vector2 element, const std::deque<Vector2> &deque);
bool eventTriggered(double interval);