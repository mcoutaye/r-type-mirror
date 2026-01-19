/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Wave System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/EntityFactory.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

class WaveSystem : public ISystem {
public:
    WaveSystem(ECS& ecs) : ISystem(ecs) {}

    void loadLevel(const std::vector<WaveData_t>& waves);
    void update(double dt) override;

private:
    std::vector<WaveData_t> _waves;
    size_t _currentWave = 0;
    float _timer = 0.f;
};

void WaveSystem::loadLevel(const std::vector<WaveData_t>& waves)
{
    _waves = waves;
    _currentWave = 0;
    _timer = _waves.empty() ? 0.f : _waves[0].delay; // ← "delay" pas "time" (cohérent avec Components.hpp)
    if (!_waves.empty())
        std::cout << "[WaveSystem] Level loaded - " << _waves.size() << " waves planned\n";
}

void WaveSystem::update(double dt)
{
    if (_currentWave >= _waves.size())
        return;

    _timer -= static_cast<float>(dt);

    if (_timer <= 0.f) {
        const WaveData_t& wave = _waves[_currentWave];

        std::cout << "[WaveSystem] Spawning wave " << _currentWave + 1
                  << " - " << wave.count << " enemies\n";

        for (int i = 0; i < wave.count; ++i) {
            float x = wave.x + (i * 100.f); // Espacement des ennemis
            Factory::createEnemy(_ecs, x, wave.y, wave.movementType);
        }

        ++_currentWave;

        // Prochaine vague
        if (_currentWave < _waves.size())
            _timer = _waves[_currentWave].delay;
        else
            _timer = 10.f; // plus de vagues
    }
}