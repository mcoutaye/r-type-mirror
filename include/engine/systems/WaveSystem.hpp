/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Wave System
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
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
            Entity enemy = _ecs.createEntity();

            float offsetX = i * 70.f;           // espacement horizontal
            float offsetY = (i % 4) * 60.f;     // un peu de dispersion verticale

            _ecs.addComponent(enemy, Position_t{wave.x + offsetX, wave.y + offsetY});
            _ecs.addComponent(enemy, Velocity_t{-180.f - i * 10.f, 0.f});

            // Pattern de mouvement (alternance sinus/cosinus)
            MovementPattern_t pattern;
            switch (i % 6) {
                case 0:
                    pattern.type = MovementPattern_t::Type::Linear;
                    break;
                case 1:
                    pattern.type = MovementPattern_t::Type::Sinus;
                    break;
                case 2:
                    pattern.type = MovementPattern_t::Type::Cosinus;
                    break;
                case 3:
                    pattern.type = MovementPattern_t::Type::Circle;
                    break;
                case 4:
                    pattern.type = MovementPattern_t::Type::Zigzag;
                    break;
                case 5:
                    pattern.type = MovementPattern_t::Type::Spiral;
                    break;
            }
            pattern.amplitude = 50.f;  // Amplitude de l'onde
            pattern.frequency = 1.f;    // Fréquence de l'onde
            pattern.radius = 30.f;      // Rayon pour Circle/Spiral
            pattern.speed = 2.f;        // Vitesse pour Circle/Zigzag/Spiral
            _ecs.addComponent(enemy, pattern);

            // Autres composants
            _ecs.addComponent(enemy, Drawable_t{"enemy.png", {0, 0, 64, 64}, 10, true});
            _ecs.addComponent(enemy, Collider_t{50.f, 50.f, true, 2, 15});
            _ecs.addComponent(enemy, Health_t{40, 40});
        }

        ++_currentWave;

        // Prochaine vague
        if (_currentWave < _waves.size())
            _timer = _waves[_currentWave].delay;
        else
            _timer = 99999.f; // plus de vagues
    }
}