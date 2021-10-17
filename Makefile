CC=gcc
OBJECTS=main.o tetris.o
CFLAGS=-Wall
LIBS=-lSDL2 -lm

tetris: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o tetris

.PHONY: run
run: tetris
	@echo =====Running=====
	@./tetris

.PHONY: clean
clean:
	rm *.o
	rm tetris
