#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "Definitions.h"
#include "Game.h"

using namespace std;

enum Color { Red, Green, Blue, Cyan, Magenta, Yellow, Orange, Black, Gray };

color4 colors[9] = {
    color4(1.0, 0.0, 0.0, 1.0),    color4(0.0, 1.0, 0.0, 1.0),
    color4(0.0, 0.0, 1.0, 1.0),    color4(0.0, 1.0, 1.0, 1.0),
    color4(1.0, 0.0, 1.0, 1.0),    color4(1.0, 1.0, 0.0, 1.0),
    color4(1.0, 0.65, 0.0, 1.0),   color4(0.0, 0.0, 0.0, 1.0),
    color4(0.20, 0.20, 0.20, 1.0),
};

enum Face {
  Front = 0,
  Back = 1,
  Right = 2,
  Left = 3,
  Up = 4,
  Down = 5,
};

point4 *vertices;
color4 *vertexColors;
int numVertices;

GLuint GlobalModelViewLoc, ProjectionLoc;
mat4 globalModelView;
mat4 projection;

GLuint ModelViewLoc;
mat4 modelView;

GLuint ColorLoc, TextSwitchLoc;

Game game;

int mode = 0;

int xOriginal, yOriginal;
double dThetaCurrent = 0, dPhiCurrent = 0, dTheta = 0, dPhi = 0;

GLfloat squareVertices[][2] = {{-0.5, -0.5}, {-0.5, 0.5},  {0.5, 0.5},
                               {0.5, -0.5},  {-0.5, -0.5}, {0.5, 0.5}};

bool paused = false;

void colorSquare(Color color) {
  size_t sizeVertices, sizeVertexColors;
  sizeVertices = numVertices * sizeof(*vertices);
  sizeVertexColors = numVertices * sizeof(*vertexColors);

  for (int j = 0; j < 6; ++j) {
    for (int i = 0; i < 6; ++i) {
      vertexColors[j * 6 + i] = colors[color];
      if (j != 0) {
        vertexColors[j * 6 + i][3] = 0.0;
      } else {
        vertexColors[j * 6 + i][3] = 0.2;
      }
    }
  }

  glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeVertexColors,
                  vertexColors);
}

void setBlockColor(Color color, float alpha = 1.0) {
  size_t sizeVertices, sizeVertexColors;
  sizeVertices = numVertices * sizeof(*vertices);
  sizeVertexColors = numVertices * sizeof(*vertexColors);

  for (int i = 0; i < numVertices; ++i) {
    vertexColors[i] = colors[color];
    vertexColors[i][3] = alpha;
  }
  glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeVertexColors,
                  vertexColors);
}

void drawPiece(Tetromino &p, int x, int y) {
  GLfloat t = 1.0 + GAP;
  int size = p.size();

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (p.blocks[i][j] != 0) {
        modelView = Translate(t * (x + j), t * (y - i), 0);
        glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, modelView);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
      }
    }
  }
}

void drawFalling() {
  Tetromino p = game.fallingPiece;

  setBlockColor((Color)p.type, 0.8);
  drawPiece(p, game.pieceX, game.pieceY);
}

void drawProjection() {
  Board board = game.board;
  Tetromino p = game.fallingPiece;

  int projY = game.pieceY;

  while (board.checkCollision(p, game.pieceX, projY - 1) == Collision::Free) {
    projY -= 1;
  }

  setBlockColor((Color)p.type, 0.2);
  drawPiece(p, game.pieceX, projY);
}

void drawNext() {
  Tetromino p = game.nextPiece;

  setBlockColor((Color)p.type, 0.8);
  drawPiece(p, BOARD_WIDTH + 2, BOARD_HEIGHT - 2);
}

void drawBoard() {
  GLfloat t = 1.0 + GAP;

  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      int gridValue = game.board.grid[i][j];
      if (gridValue == 0) {
        // setBlockColor(Gray, 0.1);
        colorSquare(Gray);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        modelView = Translate(t * j, t * i, -t);
        glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, modelView);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
      } else {
        setBlockColor((Color)(gridValue - 1), 0.8);
        modelView = Translate(t * j, t * i, 0);
        glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, modelView);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
      }
      // modelView = Translate(t * j, t * i, 0);
    }
  }
}

void printInfo() {
  string text[1];
  ostringstream out;
  out << "Score: " << game.score << " | Level: " << game.level
      << " | Rows: " << game.rows;
  text[0] = out.str();

  mat4 prevModelView = modelView;
  glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, Translate(-5, 0, 0));
  mat4 prevGlobalModelView = globalModelView;
  glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, mat4(1.0));
  mat4 prevProjection = projection;
  glUniformMatrix4fv(ProjectionLoc, 1, GL_TRUE, Ortho2D(-12, 12, -12, 12));
  glUniform4fv(ColorLoc, 1, color4(1.0, 1.0, 1.0, 1.0));
  glUniform1i(TextSwitchLoc, 1);

  glDisable(GL_DEPTH_TEST);
  GLfloat x = TEXT_X, y = TEXT_Y;

  GLfloat step = (LINE_HEIGHT / (GLfloat)glutGet(GLUT_INIT_WINDOW_HEIGHT) /
                  ((GLfloat)glutGet(GLUT_WINDOW_HEIGHT) /
                   (GLfloat)glutGet(GLUT_INIT_WINDOW_HEIGHT)));
  glRasterPos2f(x, y);
  size_t size = sizeof(text) / sizeof(text[0]);
  for (int i = size - 1; i >= 0; i--) {
    for (string::iterator j = text[i].begin(); j != text[i].end(); ++j) {
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *j);
    }
    glRasterPos2f(x, y + step * (size - i));
  }
  glEnable(GL_DEPTH_TEST);
  glUniform1i(TextSwitchLoc, 0);

  // Restore the modelview and the projection matrices
  modelView = prevModelView;
  glUniformMatrix4fv(ModelViewLoc, 1, GL_TRUE, modelView);
  globalModelView = prevGlobalModelView;
  glUniformMatrix4fv(GlobalModelViewLoc, 1, GL_TRUE, globalModelView);
  projection = prevProjection;
  glUniformMatrix4fv(ProjectionLoc, 1, GL_TRUE, projection);
}

int readOFF(string filename) {
  ifstream file(filename);
  string line;

  file >> line;
  if (line != "OFF") {
    cerr << filename << ": Not an OFF file\n";
    return 1;
  }

  int numPoints, numFaces;
  file >> numPoints >> numFaces;
  file.ignore(numeric_limits<streamsize>::max(), '\n');
  point4 *points = new point4[numPoints];

  GLfloat x, y, z;

  for (int i = 0; i < numPoints; ++i) {
    file >> x >> y >> z;
    points[i] = point4(x, y, z, 1.0);
  }

  int numIndices;
  file >> numIndices;
  file.seekg(-1, ios_base::cur);

  numVertices = numFaces * numIndices;
  vertices = new point4[numVertices];

  int index;

  for (int i = 0; i < numVertices; i += numIndices) {
    file.ignore(numeric_limits<streamsize>::max(), ' ');

    for (int j = 0; j < numIndices; ++j) {
      file >> index;
      vertices[i + j] = points[index];
    }
  }

  delete[] points;

  return 0;
}

void init() {
  if (readOFF("src/cube.off") == 1) {
    exit(EXIT_FAILURE);
  }
  vertexColors = new color4[numVertices];
  setBlockColor(Black);
  game.createPiece();
  // game.fallingPiece.print();

  GLuint array;
  glGenVertexArrays(1, &array);
  glBindVertexArray(array);

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  size_t sizeVertices, sizeVertexColors;
  sizeVertices = numVertices * sizeof(*vertices);
  sizeVertexColors = numVertices * sizeof(*vertexColors);
  glBufferData(GL_ARRAY_BUFFER,
               numVertices * (sizeof(*vertices) + sizeof(*vertexColors)), NULL,
               GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeVertexColors,
                  vertexColors);

  GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");

  GLuint vPosition = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  GLuint vColor = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(vColor);
  glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                        BUFFER_OFFSET(sizeVertices));

  GlobalModelViewLoc = glGetUniformLocation(program, "GlobalModelView");
  ProjectionLoc = glGetUniformLocation(program, "Projection");

  ModelViewLoc = glGetUniformLocation(program, "ModelView");

  ColorLoc = glGetUniformLocation(program, "uColor");
  TextSwitchLoc = glGetUniformLocation(program, "TextSwitch");

  glUseProgram(program);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);

  glClearColor(0.08, 0.08, 0.08, 1.0);

  float delta_x = BOARD_WIDTH / 2.0;
  float delta_y = BOARD_HEIGHT / 2.0;
  globalModelView = Translate(-delta_x, -delta_y, 0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // float delta_x = BOARD_WIDTH / 2.0;
  // float delta_y = BOARD_HEIGHT / 2.0;
  // globalModelView = RotateX(30) * RotateY(30) * Translate(-delta_x, -delta_y,
  // 0); globalModelView = Translate(-delta_x, -delta_y, 0);
  double t = dThetaCurrent + dTheta;
  double p = dPhiCurrent + dPhi;
  vec4 eye(-sin(t) * cos(p), -sin(p), cos(t) * cos(p), 1.0);
  vec4 at(0.0, 0.0, 0.0, 1.0);
  vec4 up(-sin(p) * sin(t), cos(p), sin(p) * cos(t), 0.0);

  vec4 eyex(-sin(t), 0.0, cos(t), 1.0);
  vec4 upx(0.0, 1.0, 0.0, 1.0);
  vec4 eyey(0.0, -sin(p), cos(p), 1.0);
  vec4 upy(0.0, cos(p), sin(p), 1.0);
  float delta_x = BOARD_WIDTH / 2.0;
  float delta_y = BOARD_HEIGHT / 2.0;
  globalModelView = (LookAt(eye, at, up) * Translate(-delta_x, -delta_y, 0));
  glUniformMatrix4fv(GlobalModelViewLoc, 1, GL_TRUE, globalModelView);

  printInfo();
  if (!game.board.gameOver()) {
    drawFalling();
    drawNext();
    drawProjection();
  }
  drawBoard();

  glutSwapBuffers();
}

void reshape(int width, int height) {
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  GLfloat size = 12.0;
  // Preserve the aspect ratio of the object
  if (width <= height) {
    projection = Ortho(-size, size, -size * (GLfloat)height / (GLfloat)width,
                       size * (GLfloat)height / (GLfloat)width, -size, size);
  } else {
    projection = Ortho(-size * (GLfloat)width / (GLfloat)height,
                       size * (GLfloat)width / (GLfloat)height, -size, size,
                       -size, size);
  }

  glUniformMatrix4fv(ProjectionLoc, 1, GL_TRUE, projection);
}

void update(int p) {
  Board &board = game.board;
  Tetromino current = game.fallingPiece;
  int pieceX = game.pieceX;
  int pieceY = game.pieceY;

  board.print();
  cout << endl;
  if (!board.gameOver() && !paused) {
    if (board.checkCollision(current, pieceX, pieceY - 1) == Collision::Free) {
      game.pieceY -= 1;
    } else {
      board.insert(current, game.pieceX, game.pieceY);
      game.clearRows();
      game.createPiece();
    }

    glutPostRedisplay();
    glutTimerFunc(game.stepTime, update, 0);
  } else {
    glutPostRedisplay();
  }
}

void keyboard(unsigned char key, int x, int y) {
  Board &board = game.board;
  Tetromino current = game.fallingPiece;

  switch (key) {
  case 32:
    if (!board.gameOver() && !paused) {
      while (board.checkCollision(current, game.pieceX, game.pieceY - 1) ==
             Collision::Free) {
        game.pieceY -= 1;
      }
      board.insert(current, game.pieceX, game.pieceY);
      game.clearRows();
      game.createPiece();
    }
    break;
  case 'r':
    if (board.gameOver()) {
      glutTimerFunc(game.stepTime, update, 0);
    }
    game.restart();
    break;
  case 'v':
    dThetaCurrent = 0.0;
    dPhiCurrent = 0.0;
    // float delta_x = BOARD_WIDTH / 2.0;
    // float delta_y = BOARD_HEIGHT / 2.0;
    // globalModelView = Translate(-delta_x, -delta_y, 0);
    break;
  case 'p':
    paused = !paused;
    if (!paused)
      glutTimerFunc(game.stepTime, update, 0);
    break;
  }

  glutPostRedisplay();
}

void special(int key, int x, int y) {
  if (paused) {
    return;
  }

  Board &board = game.board;
  Tetromino current = game.fallingPiece;
  int pieceX = game.pieceX;
  int pieceY = game.pieceY;

  Collision status;
  Tetromino rotated;

  switch (key) {
  case GLUT_KEY_LEFT:
    status = board.checkCollision(current, pieceX - 1, pieceY);
    if (status == Collision::Free) {
      game.pieceX -= 1;
    }
    break;
  case GLUT_KEY_RIGHT:
    status = board.checkCollision(current, pieceX + 1, pieceY);
    if (status == Collision::Free) {
      game.pieceX += 1;
    }
    break;
  case GLUT_KEY_UP:
    rotated = current.rotate(Direction::CCW);
    status = board.checkCollision(rotated, pieceX, pieceY);
    if (status == Collision::Free) {
      game.fallingPiece = rotated;
    }
    // board.insert(current, pieceX, pieceY);
    // board.print();
    break;
  case GLUT_KEY_DOWN:
    status = board.checkCollision(current, pieceX, pieceY - 1);
    if (status == Collision::Free) {
      game.pieceY -= 1;
    }
    break;
  }

  glutPostRedisplay();
}

void mouse(int button, int button_state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (button_state == GLUT_DOWN) {
      // Record where the left mouse button was pressed
      xOriginal = x;
      yOriginal = y;
    } else {
      // Record where the left mouse button was depressed
      dThetaCurrent += dTheta;
      dPhiCurrent += dPhi;

      dTheta = 0.0;
      dPhi = 0.0;
    }
  }
}

void motion(int x, int y) {
  // We scale by MOUSE_SCALE to slow it down a bit and divide by
  // state.scaleFactor to negate the scaling of the rotation matrices with
  // the scale matrix in the display callback function, i.e. we want the
  // rotation with respect to mouse movement to be independent of the zoom
  // level.
  dTheta = (x - xOriginal) * MOUSE_SCALE;
  dPhi = (yOriginal - y) * MOUSE_SCALE;

  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);
  glutCreateWindow(WINDOW_NAME);

#ifdef __linux__
  glewExperimental = GL_TRUE;
  glewInit();
#endif

  srand(time(NULL));
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(game.stepTime, update, 0);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();

  return 0;
}
