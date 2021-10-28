CC=gcc
OBJECTS=main.o tetris.o render.o
CFLAGS=-Wall
LIBS=-lSDL2 -lm

tetris: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o tetris

*.o: *.h

.PHONY: run
run: tetris
	@echo =====Running=====
	@./tetris

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) tetris
