// include/WaveSystem.hpp
#pragma once
#include "ecs.hpp"
#include <vector>
#include "Components.hpp"   // WaveData est ici

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