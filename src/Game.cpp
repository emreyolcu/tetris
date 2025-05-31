#include <cmath>
#include <stdlib.h>
#include <time.h>

#include "Game.h"

Game::Game() {
  stepTime = 500;
  score = 0;
  rows = 0;
  level = 1;
  // cout << nextPiece.size() << endl;
}

void Game::createPiece() {
  Type next = static_cast<Type>(rand() % static_cast<int>(Type::N_TYPES));
  if (nextPiece.size() == 0) {
    Type falling = static_cast<Type>(rand() % static_cast<int>(Type::N_TYPES));

    fallingPiece = Tetromino(falling);
  } else {
    fallingPiece = nextPiece;
  }
  nextPiece = Tetromino(next);

  int fallingSize = fallingPiece.size();
  Type fallingType = fallingPiece.type;

  pieceX = (BOARD_WIDTH - fallingSize) / 2;
  pieceY = BOARD_HEIGHT - (fallingType == Type::I ? 0 : 1);
}

void Game::restart() {
  board.clear();
  createPiece();
  score = 0;
  rows = 0;
  level = 1;
  stepTime = 500;
}

void Game::clearRows() {
  int rowsCleared = board.clearRows();

  rows += rowsCleared;
  if (rowsCleared != 0) {
    score += pow(5, rowsCleared);
  }

  if ((rows / 10) + 1 > level) {
    ++level;
    stepTime *= 0.8;
  }
}
