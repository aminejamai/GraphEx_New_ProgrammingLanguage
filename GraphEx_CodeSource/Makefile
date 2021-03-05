OBJS = main.c scanner.c parser.c

CC = gcc

LIBRARY_PATHS = -LC:\MinGW\lib

COMPILER_FLAGS = -Wall -Wextra

OBJ_NAME = gx

all: $(OBJS)
	$(CC) $(OBJS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) -o $(OBJ_NAME)

clean:
	rm -r $(OBJ_NAME)
