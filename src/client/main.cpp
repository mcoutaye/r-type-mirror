/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Client
*/

#include "../engine/core/GameEngine.hpp"
#include <iostream>

int main() {
    try {
        GameEngine engine;
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}