#include "Tetromino.h"
#include "Definitions.h"

Tetromino::Tetromino() {
  //
}

int Tetromino::size() { return blocks.size(); }

Tetromino::Tetromino(Type t) {
  type = t;

  switch (type) {

  case Type::I:
    blocks = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    break;

  case Type::O:
    blocks = {{1, 1}, {1, 1}};
    break;

  case Type::T:
    blocks = {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}};
    break;

  case Type::J:
    blocks = {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}};
    break;

  case Type::L:
    blocks = {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}};
    break;

  case Type::S:
    blocks = {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}};
    break;

  case Type::Z:
    blocks = {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}};
    break;

  default:
    break;
  }

  int size = this->size();

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      blocks[i][j] *= static_cast<int>(t) + 1;
    }
  }
}

Tetromino Tetromino::rotate(Direction d) {
  int size = this->size();

  Tetromino rotated = Tetromino(type);

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (d == Direction::CW) {
        rotated.blocks[j][i] = blocks[size - 1 - i][j];
      } else if (d == Direction::CCW) {
        rotated.blocks[j][i] = blocks[i][size - 1 - j];
      }
    }
  }

  return rotated;
}

void Tetromino::print() {
  int size = this->size();

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      cout << this->blocks[i][j] << " ";
    }
    cout << endl;
  }
}
