#include <string>

#include "Board.h"

Board::Board() { clear(); }

void Board::clear() {
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      grid[i][j] = 0;
    }
  }
}

Collision Board::checkCollision(Tetromino &t, int x, int y) {
  int size = t.size();

  for (int dy = 0; dy < size; ++dy) {
    for (int dx = 0; dx < size; ++dx) {
      int cx = x + dx;
      int cy = y - dy;
      if (t.blocks[dy][dx] != 0) {
        if (cx < 0 || cx >= BOARD_WIDTH || cy < 0 || grid[cy][cx] != 0) {
          return Collision::Blocked;
        }
      }
    }
  }

  return Collision::Free;
}

bool Board::gameOver() {
  for (int x = 0; x < BOARD_WIDTH; ++x) {
    if (grid[BOARD_HEIGHT - 2][x] != 0) {
      return true;
    }
  }

  return false;
}

int Board::clearRows() {
  int rowsCleared = 0;
  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    bool filled = true;

    for (int x = 0; x < BOARD_WIDTH; ++x) {
      if (grid[y][x] == 0) {
        filled = false;
      }
    }

    if (filled) {
      for (int cy = y; cy < BOARD_HEIGHT - 1; ++cy) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
          grid[cy][x] = grid[cy + 1][x];
        }
      }

      ++rowsCleared;
      --y;
    }
  }

  return rowsCleared;
}

void Board::insert(Tetromino &t, int x, int y) {
  int size = t.size();

  for (int dy = 0; dy < size; ++dy) {
    for (int dx = 0; dx < size; ++dx) {
      int cx = x + dx;
      int cy = y - dy;
      if (t.blocks[dy][dx] != 0) {
        grid[cy][cx] = t.blocks[dy][dx];
      }
    }
  }
}

void Board::print() {
  for (int i = BOARD_HEIGHT - 1; i >= 0; --i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      int gridValue = this->grid[i][j];
      string printValue = gridValue == 0 ? "_" : to_string(gridValue);
      cout << printValue << " ";
    }
    cout << endl;
  }
}
