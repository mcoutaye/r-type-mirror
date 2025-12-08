/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game Engine
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "engine/systems/InputSystem.hpp"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void run(); 

private:
    void processInputs();
    void update();
    void render();

    sf::RenderWindow window;
    InputSystem inputSystem;
    bool isRunning;
};