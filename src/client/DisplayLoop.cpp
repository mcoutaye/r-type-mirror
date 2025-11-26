/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** DisplayLoop
*/

#include "DisplayLoop.hpp"
#include <iostream>

DisplayLoop::DisplayLoop() = default;

DisplayLoop::~DisplayLoop()
{
    stop();
}

void DisplayLoop::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&DisplayLoop::displayLoop, this);
    }
}

void DisplayLoop::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void DisplayLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void DisplayLoop::displayLoop()
{
    std::cout << "[DisplayLoop] Started - 60 FPS target\n";
    while (_running) {
        float dt = _clock.restart().asSeconds();
        (void)dt;
        // TODO: Ajouter ici la logique de rendu/affichage du jeu
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[DisplayLoop] Stopped\n";
}
