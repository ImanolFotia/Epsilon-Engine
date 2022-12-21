//
// Created by solaire on 12/15/22.
//

#include <iostream>

#include "chess.hpp"

int main(int argc, char** argv) {

    framework::env::setArgs(argc, argv);

    ChessApp::ChessApp app("Chess");

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}