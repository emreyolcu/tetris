#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "Angel.h"

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#define INIT_WINDOW_WIDTH 500
#define INIT_WINDOW_HEIGHT 800
#define WINDOW_NAME "Tetris"

#define GAP 0.05f

#define TEXT_X -0.99f
#define TEXT_Y -0.98f
#define LINE_HEIGHT 32

#define MOUSE_SCALE 0.01f

typedef vec4 point4;
typedef vec4 color4;

#endif
