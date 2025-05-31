CC = g++
CFLAGS = -std=c++11 -Wall -Wno-deprecated -pedantic -Iinclude
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LDFLAGS += -lGL -lGLU -lglut -lGLEW
else ifeq ($(UNAME), Darwin)
	LDFLAGS = -framework OpenGL -framework GLUT
endif

HEADERS = include/Angel.h include/vec.h include/mat.h

play: bin/Main.o bin/Game.o bin/Board.o bin/Tetromino.o bin/InitShader.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/Main.o: src/Main.cpp $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

bin/InitShader.o: src/InitShader.cpp $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

bin/%.o: src/%.cpp src/%.h $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f play bin/*
