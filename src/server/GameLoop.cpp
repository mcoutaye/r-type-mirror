/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** GameLoop
*/

#include "GameLoop.hpp"
#include <iostream>
#include <sstream>


GameLoop::GameLoop() = default;

GameLoop::~GameLoop()
{
    stop();
}

void GameLoop::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&GameLoop::gameLoop, this);
    }
}

void GameLoop::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::join()
{
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::gameLoop()
{
    std::cout << "[GameLoop] Started - 60 FPS target\n";
    while (_running) {
        float dt = _clock.restart().asSeconds();
        (void)dt;
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "[GameLoop] Stopped\n";
}
