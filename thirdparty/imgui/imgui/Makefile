# Simple makefile to build inih as a static library using g++

SRC = imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp
OBJ = $(SRC:.cpp=.o)
OUT = libimgui_test.a
INCLUDES = ./
CCFLAGS = -g -O2
CC = g++

default: $(OUT)

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $(EXTRACCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ) $(EXTRAARFLAGS)

clean:
	rm -f $(OBJ) $(OUT)