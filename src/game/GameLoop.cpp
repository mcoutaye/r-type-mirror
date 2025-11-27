/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** GameLoop
*/

#include "../../include/game/GameLoop.hpp"

GameLoop::GameLoop(Ntw::UdpSender& sender) : _sender(sender) {}

GameLoop::~GameLoop() {
    stop();
}

void GameLoop::start() {
    if (!_running) {
        _running = true;
        _thread = std::thread([this]() { this->gameLoop(); });
    }
}

void GameLoop::stop() {
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::join() {
    if (_thread.joinable())
        _thread.join();
}

void GameLoop::setOnPositionUpdate(OnPositionUpdate callback) {
    _onPositionUpdate = callback;
}

