/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Wave System
*/

#pragma once
#include "engine/ecs/ecs.hpp"
#include "engine/systems/Components.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

class WaveSystem : public ISystem {
public:
    WaveSystem(ECS& ecs) : ISystem(ecs) {}

    void loadLevel(const std::vector<WaveData>& waves);
    void update(double dt) override;

private:
    std::vector<WaveData> _waves;
    size_t _currentWave = 0;
    float _timer = 0.f;
};