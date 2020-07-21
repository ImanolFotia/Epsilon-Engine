# Simple makefile to build inih as a static library using g++

SRC = ./src/glad.c
OBJ = $(SRC:.c=.o)
OUT = lib/libglad.a
INCLUDES = -I./include
CCFLAGS = -g -O2
CC = g++

default: $(OUT)

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $(EXTRACCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	-@mkdir -p ./lib
	ar rcs $(OUT) $(OBJ) $(EXTRAARFLAGS)

clean:
	rm -f $(OBJ) $(OUT)
