//
// Created by solaire on 12/22/22.
//

#ifndef EPSILON_UCI_HPP
#define EPSILON_UCI_HPP

#include <iostream>
#include <string>

#include "utils/popen2.hpp"

class UCI {
public:
    UCI() = default;

    std::string last_move = "";
    popen2_t child;
    bool think = false;

    void init() {
        popen2("./build/examples/chess/stockfish/stockfish-ubuntu-20.04-x86-64-avx2", &child);

        std::cout << "Launched: ";
        std::cout << read_from_pipe(child.from_child) << std::endl;
        std::cout << "With PID: " << child.child_pid << std::endl;

        write_to_pipe(child.to_child, "uci\n");

        std::cout << read_from_pipe(child.from_child) << std::endl;

        write_to_pipe(child.to_child, "isready\n");

        std::cout << read_from_pipe(child.from_child) << std::endl;

        write_to_pipe(child.to_child, "ucinewgame\n");


        while (1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (think) {
                std::string output;
                output = read_from_pipe(child.from_child);
                std::cout << output << std::endl;
                if (output.find("bestmove") != std::string::npos) {
                    std::string best_move = output.substr(output.find("bestmove") + 9, 4);
                    std::cout << "best move found: " << best_move << std::endl;
                    last_move = best_move;
                    think = false;
                }
            }
        }
    }

    void Move(std::string move) {
        std::cout << move << std::endl;
        write_to_pipe(child.to_child, move + "\n");
        write_to_pipe(child.to_child, "go depth 20\n");
        think = true;
    }
};

#endif //EPSILON_UCI_HPP
