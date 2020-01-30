#!/bin/bash

# Makefile for 64bit build of Epsilon Engine, by Imanol Fotia
# Dependencies need to be downloaded under Windows
# For my build I'm using g++ 9.2.0

# Dependencies: 
# Bullet Physics 3
# Glew
# GLFW
# inih
# OpenAL Soft
# Lua 5.3.5
# SOIL (soon to be replaced)

CXX= g++

OBJS_DIR:= ./obj
SOURCE_DIR:= ./core/src
INCLUDE_DIR:= ./core/include
LIB:= deps-64
BIN:= ./bin

#recursive wildcard to include all files
rwildcard=$(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))

SOURCES:= $(call rwildcard,$(SOURCE_DIR)/,*.cpp) #$(wildcard $(SOURCE_DIR)/**.cpp) 
INCLUDES:= $(call rwildcard,$(INCLUDE_DIR)/,*.hpp *.h)#$(wildcard $(INCLUDE_DIR)/**.hpp $(INCLUDE_DIR)/**.h)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SOURCES))
RES_OBJECTS := $(OBJS_DIR)/resources.o

INCLUDE_LIBS:= -I$(LIB)/glm \
-I$(LIB)/bullet3/src \
-I$(LIB)/glew/include -I$(LIB)/glfw/include \
-I$(LIB)/soil/Simple\ OpenGL\ Image\ Library/src \
-I$(LIB)/openal-soft/include \
-I$(LIB)/inih/cpp \
-I$(LIB)/lua-5.3.5/src \

LIBS_DIR := -L$(LIB)/soil/lib \
-L$(LIB)/glfw/build/src \
-L$(LIB)/inih \
-L$(LIB)/glew/lib \
-L$(LIB)/lua-5.3.5/src \
-L$(LIB)/openal-soft/build \
-L$(LIB)/bullet3/build/lib

LIBS:= -lSOIL -lglfw3dll -lopengl32 -linih -lglew32.dll -lgdi32 -lOpenAL32.dll -fopenmp -lgomp -llua -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath

LD_FLAGS := -fopenmp
CPPFLAGS := --std=c++17 -Wall

DEBUG_FLAGS := -g -DDEBUG

RELEASE_FLAGS := -O2

RES := ./core/src/resources.rc

all: resource epsilon-release

epsilon-debug: resource $(BIN)/Debug/Epsilon_Engine.exe

epsilon-release: resource $(BIN)/Release/Epsilon_Engine.exe

resource:
	@windres -i $(RES) -o $(OBJS_DIR)/resources.o

$(OBJS_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	-@mkdir -p $(@D)
	$(CXX) $(INCLUDE_LIBS)  $(DEBUG_FLAGS) -I$(INCLUDE_DIR) $(CPPFLAGS) -o $@ -c $^  $(LD_FLAGS) 

$(BIN)/Release/Epsilon_Engine.exe: $(OBJECTS)
	-@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(RELEASE_FLAGS) $(RES_OBJECTS) -o $@ $^ $(LIBS_DIR) $(LIBS) $(LD_FLAGS)

$(BIN)/Debug/Epsilon_Engine.exe: $(OBJECTS)
	-@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(DEBUG_FLAGS) $(RES_OBJECTS) -o $@ $^ $(LIBS_DIR) $(LIBS) $(LD_FLAGS)

clean:
	-@rm -rf ./obj/*

