#!/bin/bash

EXE = bin/DDSViewer.exe
LIBS :=  -L../../deppend/glfw-3.2.1/build/src  -L../../deppend/glew-2.1.0/lib/ 
INCLUDES := -I../../deppend/glew-2.1.0/include -I../../deppend/glfw-3.2.1/include
SOURCES := main.cpp DDS.cpp Shader.cpp
CXXFLAGS := --std=c++11 -g -Wall
LDFLAGS :=  -static -static-libgcc -lglfw3 -lopengl32 -lglew32.dll -lgdi32

OBJ_DIR = ./obj

OBJS = $(OBJ_DIR)/DDS.o $(OBJ_DIR)/Shader.o

all: objects bin

objects: $(OBJ_DIR)/DDS.o $(OBJ_DIR)/Shader.o

bin: $(EXE)
	
$(OBJ_DIR)/DDS.o:
	-@mkdir -p obj
	$(CXX) -c DDS.cpp -o $@ $(INCLUDES) $(CXXFLAGS)
	
$(OBJ_DIR)/Shader.o:
	-@mkdir -p obj
	$(CXX) -c Shader.cpp -o $@ $(INCLUDES) $(CXXFLAGS)

$(EXE):
	-@mkdir -p bin
	$(CXX) main.cpp $(OBJS) -o $@ $(INCLUDES) $(LIBS) $(LDFLAGS) $(CXXFLAGS)