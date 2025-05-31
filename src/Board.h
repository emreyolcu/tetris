#ifndef BOARD_H
#define BOARD_H

#include "Tetromino.h"

#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10
#define BLOCK_SIZE 20

enum class Collision { Free, Blocked };

class Board {
public:
  Board();
  int grid[BOARD_HEIGHT][BOARD_WIDTH];
  Collision checkCollision(Tetromino &, int, int);
  bool gameOver();
  int clearRows();
  void insert(Tetromino &, int, int);
  void print();
  void clear();
};

#endif
