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

OBJS_DIR_RELEASE:= ./obj/Release
OBJS_DIR_DEBUG:= ./obj/Debug
SOURCE_DIR:= ./core
INCLUDE_DIR:= ./core
LIB:= thirdparty
OUT_LIB := lib
BIN:= ./bin

ifeq "$(OS)" "Windows_NT"
EXEC := Epsilon_Engine.exe
else
EXEC := Epsilon_Engine
endif

LIB_NAME:= libEpsilon.a
LIB_NAME_DEBUG:= libEpsilon_d.a

#recursive wildcard to include all files
rwildcard=$(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))

SOURCES:= $(call rwildcard,$(SOURCE_DIR)/,*.cpp) #$(wildcard $(SOURCE_DIR)/**.cpp) 
INCLUDES:= $(call rwildcard,$(INCLUDE_DIR)/,*.hpp *.h)#$(wildcard $(INCLUDE_DIR)/**.hpp $(INCLUDE_DIR)/**.h)
OBJECTS_DEBUG := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJS_DIR_DEBUG)/%.o,$(SOURCES))
OBJECTS_RELEASE := $(patsubst $(SOURCE_DIR)/%.cpp,$(OBJS_DIR_RELEASE)/%.o,$(SOURCES))
RES_OBJECTS := ./obj/resources.o

#DX_SDK_INCLUDE := /c/Program\ Files\ \\(x86\\)/Microsoft\ DirectX\ SDK\ \\(June\ 2010\\)/Include
DX_SDK_INCLUDE := H:/Epsilon/Epsilon\ Engine/thirdparty/XAudio2/microsoft.xaudio2.redist.1.2.1/build/native/include
#DX_SDK_LIB := /c/Program\ Files\ \\(x86\\)/Microsoft\ DirectX\ SDK\ \\(June\ 2010\\)/Lib/x64
DX_SDK_LIB := H:/Epsilon\Epsilon\ Engine/thirdparty/XAudio2/microsoft.xaudio2.redist.1.2.1/build/native/release/lib/x64
#- sudo add-apt-repository http://ppa.launchpad.net/keithw/glfw3/ubuntu -y
#includes for windows and linux combined

ifeq "$(OS)" "Windows_NT"
INCLUDE_LIBS:= -I$(LIB)/glm \
-I$(LIB)/bullet3/src \
-I$(LIB)/glew/include -I$(LIB)/glfw/include \
-I$(LIB)/soil/Simple\ OpenGL\ Image\ Library/src \
-I$(LIB)/openal-soft/include \
-I$(LIB)/inih/cpp \
-I$(LIB)/lua-5.3.5/src \
-I$(LIB)/stb-master \
-I$(LIB)json-develop/include 
else
INCLUDE_LIBS:=  -I$(LIB)/inih/cpp \
-I$(LIB)/glm \
-I$(LIB)json-develop/include \
-I/core/include \
-I/usr/include \
-I/usr/include/bullet \
-I/usr/include/glew \
-I/usr/include/glfw \
-I/usr/include/stb \
-I/usr/include/SOIL \
-I/usr/include/inih \
-I/usr/include/lua5.3 
endif

ifeq "$(OS)" "Windows_NT"
LIBS_DIR := -L$(LIB)/soil/lib \
-L$(LIB)/glfw/build/src \
-L$(LIB)/inih \
-L$(LIB)/glew/lib \
-L$(LIB)/lua-5.3.5/src \
-L$(LIB)/openal-soft/build \
-L$(LIB)/bullet3/build/lib \
-L$(DX_SDK_LIB)
else 
LIBS_DIR := -L$(LIB)/inih \
-L/usr/lib/x86_64-linux-gnu 
endif

ifeq "$(OS)" "Windows_NT"
LIBS:= -lSOIL -lglfw3dll -lopengl32 -linih -lglew32.dll -lgdi32 -lOpenAL32.dll -llua -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else 
LIBS:= -lSOIL -lglfw -linih -lGLEW -lGLU -lGL -lopenal -llua5.3 -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
endif

LD_FLAGS := -fopenmp
CPPFLAGS := --std=c++17 -static -static-libgcc -static-libstdc++

DEBUG_FLAGS := -g -DDEBUG -ggdb -g3 -gdwarf-4 -fvar-tracking-assignments

RELEASE_FLAGS := -O3

RES := ./core/resources.rc

all: clean resource epsilon-release

epsilon-debug: resource $(BIN)/Debug/$(EXEC)

epsilon-release: resource $(BIN)/Release/$(EXEC)

epsilon-static-lib-debug: clean $(OUT_LIB)/Debug/$(LIB_NAME_DEBUG)

epsilon-static-lib-release: clean $(OUT_LIB)/Release/$(LIB_NAME)


resource:
ifeq "$(OS)" "Windows_NT" 
	-@mkdir -p ./obj
	windres -i $(RES) -o ./obj/resources.o
else 
	-@mkdir -p ./obj
	objcopy --input binary --output pe-x86-64 --binary-architecture i386:x86-64 $(RES) ./obj/resources.o
endif

$(OBJS_DIR_DEBUG)/%.o: $(SOURCE_DIR)/%.cpp
	-@mkdir -p $(@D)/Debug
	$(CXX) $(INCLUDE_LIBS) -I$(INCLUDE_DIR) $(CPPFLAGS) -o $@ -c $^  $(LD_FLAGS) $(DEBUG_FLAGS)
	
$(OBJS_DIR_RELEASE)/%.o: $(SOURCE_DIR)/%.cpp
	-@mkdir -p $(@D)/Release
	$(CXX) $(INCLUDE_LIBS) -I$(INCLUDE_DIR) $(CPPFLAGS) -o $@ -c $^  $(LD_FLAGS)

$(BIN)/Release/$(EXEC): $(OBJECTS_RELEASE)
	-@mkdir -p $(@D)/Release
	$(CXX) $(CPPFLAGS) $(RELEASE_FLAGS) $(RES_OBJECTS) -o $@ $^ $(LIBS_DIR) $(LIBS) $(LD_FLAGS)

$(BIN)/Debug/$(EXEC): $(OBJECTS_DEBUG)
	-@mkdir -p $(@D)/Debug
	$(CXX) $(CPPFLAGS) $(DEBUG_FLAGS) $(RES_OBJECTS) -o $@ $^ $(LIBS_DIR) $(LIBS) $(LD_FLAGS)

$(OUT_LIB)/Debug/$(LIB_NAME_DEBUG): $(OBJECTS_DEBUG)
	-@mkdir -p $(@D)
	ar rcs $@ $(OBJECTS_DEBUG) 
	
$(OUT_LIB)/Release/$(LIB_NAME): $(OBJECTS_RELEASE)
	-@mkdir -p $(@D)
	ar rcs $@ $(OBJECTS_RELEASE) 

clean:
	-@rm -rf ./obj/*

