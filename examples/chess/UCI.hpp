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

    std::string output;

    void init() {
        popen2_t child;
        popen2("./stockfish/stockfish-ubuntu-20.04-x86-64-avx2", &child);

        std::cout << "Launched: ";
        std::cout << read_from_pipe(child.from_child) << std::endl;
        std::cout << "With PID: " << child.child_pid << std::endl;

        write_to_pipe(child.to_child, "uci\n");

        std::cout << read_from_pipe(child.from_child) << std::endl;

        write_to_pipe(child.to_child, "isready\n");

        std::cout << read_from_pipe(child.from_child) << std::endl;

        write_to_pipe(child.to_child, "ucinewgame\n");

        std::cout << read_from_pipe(child.from_child) << std::endl;
        }
    };

#endif //EPSILON_UCI_HPP
