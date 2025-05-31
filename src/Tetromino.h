#ifndef TETROMINO_H
#define TETROMINO_H

#include <iostream>
#include <vector>

#include "Definitions.h"

using namespace std;

enum class Type { Z, S, J, I, T, O, L, N_TYPES };
enum class Direction { CW, CCW };

class Tetromino {
public:
  Tetromino();
  Tetromino(Type);
  int size();
  Tetromino rotate(Direction);
  void print();
  Type type;
  vector<vector<int>> blocks;
};

#endif
