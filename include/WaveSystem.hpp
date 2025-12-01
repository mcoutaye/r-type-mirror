// include/WaveSystem.hpp
#pragma once
#include "ecs.hpp"
#include "Components.hpp"
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