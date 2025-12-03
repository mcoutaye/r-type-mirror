/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Game Engine
*/

#include "engine/core/GameEngine.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

GameEngine::GameEngine() : isRunning(true), window(sf::VideoMode(1280, 720), "R-Type")
{

}

GameEngine::~GameEngine()
{

}

void GameEngine::run()
{
    isRunning = true;

    while (isRunning) {
        processInputs();
        update();
        render();
    }
}


void GameEngine::processInputs()
{
    inputSystem.update(window);

    if (inputSystem.wasActionPressed(GameAction::Quit)) {
        std::cout << "ACTION: Quitter (pressé)" << std::endl;
        isRunning = false;
        window.close();
    }

    if (inputSystem.wasActionPressed(GameAction::Shoot)) {
        std::cout << "ACTION: Tirer (pressé)" << std::endl;
    }

    if (inputSystem.isActionActive(GameAction::MoveUp)) {
        std::cout << "ACTION: Bouger Haut (maintenu)" << std::endl;
    }

    if (inputSystem.isActionActive(GameAction::MoveDown)) {
        std::cout << "ACTION: Bouger Bas (maintenu)" << std::endl;
    }

    if (inputSystem.isActionActive(GameAction::MoveLeft)) {
        std::cout << "ACTION: Bouger Gauche (maintenu)" << std::endl;
    }

    if (inputSystem.isActionActive(GameAction::MoveRight)) {
        std::cout << "ACTION: Bouger Droite (maintenu)" << std::endl;
    }
}

void GameEngine::update()
{

}

void GameEngine::render()
{
    window.clear(sf::Color::Black);
    window.display();
}