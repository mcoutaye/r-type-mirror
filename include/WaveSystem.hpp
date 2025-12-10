// ============================================
// 6. WAVE SYSTEM (spawner simple)
// ============================================

// WaveSystem.hpp
#pragma once
#include "ecs.hpp"
#include <vector>

struct WaveData {
    float time;
    std::string enemyType;
    int count;
    float x, y;
};

class WaveSystem : public ISystem {
public:
    WaveSystem(ECS& ecs);
    void loadLevel(const std::vector<WaveData>& waves);
    void update(double dt) override;

private:
    std::vector<WaveData> _waves;
    size_t _currentWave = 0;
    float _waveTimer = 0.f;
    ECS& _ecs;
};