#!bin/bash
.PHONY: all clean

CXX := g++
MAKE := mingw32-make 

SOURCES := main.cpp
HEADERS :=

BUILD_DIR := ./build

LIB_DIR := ./thirdparty

LIBS := -L$(LIB_DIR)/glfw/build/src -L/c/VulkanSDK/1.3.211.0/Lib -L/c/VulkanSDK/1.3.211.0/Bin

LIBS_HEADERS := -I$(LIB_DIR)/glfw/include -IC:/VulkanSDK/1.3.211.0/Include -I$(LIB_DIR)/glm

CPPFLAGS := --std=c++20 -O2 -static -static-libgcc -static-libstdc++ 

LDFLAGS :=  "/c/VulkanSDK/1.3.211.0/Lib/vulkan-1.lib" -lglfw3dll -lpthread  -lgdi32 -lole32

all: LearningVulkan

LearningVulkan: $(BUILD_DIR)/LearningVulkan.exe

$(BUILD_DIR)/LearningVulkan.exe: main.cpp 
	-@mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS_HEADERS) $(LIBS) $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/LearningVulkan.exe