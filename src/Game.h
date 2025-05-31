#ifndef GAME_H
#define GAME_H

#include "Angel.h"
#include "Board.h"
#include "Definitions.h"
#include "Tetromino.h"

class Game {
public:
  Game();
  void createPiece();
  void restart();
  void clearRows();

  Board board;
  Tetromino fallingPiece;
  int pieceX;
  int pieceY;

  int stepTime;

  bool gameOver;

  Tetromino nextPiece;

  int score;
  int level;
  int rows;
};

#endif
