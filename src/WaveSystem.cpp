// src/WaveSystem.cpp
#include "WaveSystem.hpp"
#include "Components.hpp"      // INDISPENSABLE : Position, Velocity, Drawable, etc.
#include <iostream>
#include <vector>              // pour std::vector
#include <algorithm>           // au cas où (pas utilisé ici mais safe)

void WaveSystem::loadLevel(const std::vector<WaveData>& waves)
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
        const WaveData& wave = _waves[_currentWave];

        std::cout << "[WaveSystem] Spawning wave " << _currentWave + 1
                  << " - " << wave.count << " enemies\n";

        for (int i = 0; i < wave.count; ++i) {
            Entity enemy = _ecs.createEntity();

            float offsetX = i * 70.f;           // espacement horizontal
            float offsetY = (i % 4) * 60.f;     // un peu de dispersion verticale

            _ecs.addComponent(enemy, Position{wave.x + offsetX, wave.y + offsetY});
            _ecs.addComponent(enemy, Velocity{-180.f - i * 10.f, 0.f}); // vitesse croissante
            _ecs.addComponent(enemy, Drawable{
                "enemy.png",                    // textureId
                {0, 0, 64, 64},                 // rect
                10,                             // layer
                true                            // visible
            });
            _ecs.addComponent(enemy, Collider{50.f, 50.f, true, 2, 15}); // team 2 = ennemi
            _ecs.addComponent(enemy, Health{40, 40});
        }

        ++_currentWave;

        // Prochaine vague
        if (_currentWave < _waves.size())
            _timer = _waves[_currentWave].delay;
        else
            _timer = 99999.f; // plus de vagues
    }
}