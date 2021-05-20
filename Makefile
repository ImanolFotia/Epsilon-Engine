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
# GLAD

# define standard colors
ifneq (,$(findstring xterm,${TERM}))
	BLACK        := $(shell tput -Txterm setaf 0)
	RED          := $(shell tput -Txterm setaf 1)
	GREEN        := $(shell tput -Txterm setaf 2)
	YELLOW       := $(shell tput -Txterm setaf 3)
	LIGHTPURPLE  := $(shell tput -Txterm setaf 4)
	PURPLE       := $(shell tput -Txterm setaf 5)
	BLUE         := $(shell tput -Txterm setaf 6)
	WHITE        := $(shell tput -Txterm setaf 7)
	RESET := $(shell tput -Txterm sgr0)
else
	BLACK        := ""
	RED          := ""
	GREEN        := ""
	YELLOW       := ""
	LIGHTPURPLE  := ""
	PURPLE       := ""
	BLUE         := ""
	WHITE        := ""
	RESET        := ""
endif

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

IMGUI_INC := -I$(LIB)/imgui-docking/imgui-docking/examples \
-I$(LIB)/imgui-docking/imgui-docking

#recursive wildcard to include all files
rwildcard=$(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))

SOURCES:= $(call rwildcard,$(SOURCE_DIR)/,*.cpp)
#$(wildcard $(SOURCE_DIR)/**.cpp) 
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
-I$(LIB)/glfw/include \
-IC:/VulkanSDK/1.2.170.0/Include \
-I$(LIB)/soil/Simple\ OpenGL\ Image\ Library/src \
-I$(LIB)/openal-soft/include \
-I$(LIB)/inih/cpp \
-I$(LIB)/lua-5.3.5/src \
-I$(LIB)/stb-master \
-I$(LIB)/glad/include \
-I$(LIB)/OpenGL/include/ \
-I$(LIB)/json-develop/include \
-I$(LIB)/beacon \
-I$(LIB)/imgui-docking/imgui-docking
else
INCLUDE_LIBS:=  -I$(LIB)/inih/cpp \
-I$(LIB)/glm \
-I$(LIB)json-develop/include \
-I/core/include \
-I/usr/include \
-I/usr/include/bullet \
-I/usr/include/glfw \
-I/usr/include/stb \
-I/usr/include/SOIL \
-I/usr/include/inih \
-I$(LIB)/glad/include \
-I/usr/include/lua5.3 
endif

ifeq "$(OS)" "Windows_NT"
LIBS_DIR := -L$(LIB)/soil/lib \
-L$(LIB)/glfw/build/src \
-L$(LIB)/inih \
-L$(LIB)/lua-5.3.5/src \
-L$(LIB)/openal-soft/build \
-L$(LIB)/bullet3/build/lib \
-L$(LIB)/glad/lib \
-L$(LIB)/imgui-docking/imgui-docking \
-L$(DX_SDK_LIB)
else 
LIBS_DIR := -L$(LIB)/inih \
-L/usr/lib/x86_64-linux-gnu \
-L$(LIB)/glad/lib \
-L/usr/lib
endif

ifeq "$(OS)" "Windows_NT"
LIBS:= -lSOIL -limgui -lglfw3dll "/c/VulkanSDK/1.2.170.0/Lib/vulkan-1.lib" -lopengl32 -lglad -linih -lgdi32 -lole32 -lxaudio2_8 -lOpenAL32.dll -llua -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
else 
LIBS:= -lGLU -lGL -lSOIL -lglad -ldl -lglfw -linih -lopenal -fopenmp -static-libasan -lgomp -llua5.3 -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
endif

LD_FLAGS := -fopenmp

ifeq "$(OS)" "Windows_NT"
CPPFLAGS := --std=c++20 -static -static-libgcc -static-libstdc++
else 
CPPFLAGS := --std=c++20 -static -static-libgcc -static-libstdc++ 
endif

DEBUG_FLAGS := -g -DDEBUG -ggdb -g3 -gdwarf-4 -fvar-tracking-assignments

RELEASE_FLAGS := -O3

RES := ./core/resources.rc

all: clean resource epsilon-release

pch: $(INCLUDE_DIR)/pch.hpp.gch

epsilon-debug: pch resource $(BIN)/Debug/$(EXEC)

epsilon-release: pch resource $(BIN)/Release/$(EXEC)

epsilon-static-lib-debug: clean pch $(OUT_LIB)/Debug/$(LIB_NAME_DEBUG)

epsilon-static-lib-release: clean pch $(OUT_LIB)/Release/$(LIB_NAME)


resource:
ifeq "$(OS)" "Windows_NT" 
	-@mkdir -p ./obj
	windres -i $(RES) -o ./obj/resources.o
else 
	-@mkdir -p ./obj
	objcopy --input binary --output pe-x86-64 --binary-architecture i386:x86-64 $(RES) ./obj/resources.o
endif

$(INCLUDE_DIR)/pch.hpp.gch:
	-@mkdir -p $(@D)
	$(CXX) -x c++-header -c $(@D)/pch.hpp -o $@

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
