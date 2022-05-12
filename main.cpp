/*
    Just my adventures learning vulkan, primarily based on https://vulkan-tutorial.com/
    The makefile is for MSYS2 MingW
*/

#include "LearningVulkanApplication.hpp"

int main(int argc, char** argv) {

    LearningVulkan::LearningVulkanApplication app("LearningVulkan");

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}