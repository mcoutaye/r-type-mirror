// WaveSystem.cpp (extrait)
WaveSystem::WaveSystem(ECS& ecs) : ISystem(ecs), _ecs(ecs) {}

void WaveSystem::loadLevel(const std::vector<WaveData>& waves) {
    _waves = waves;
    _currentWave = 0;
    _waveTimer = _waves.empty() ? 0.f : _waves[0].time;
}

void WaveSystem::update(double dt) {
    _waveTimer -= dt;

    if (_waveTimer <= 0.f && _currentWave < _waves.size()) {
        // Spawn la vague
        auto& wave = _waves[_currentWave];
        for (int i = 0; i < wave.count; ++i) {
            Entity e = _ecs.createEntity();
            _ecs.addComponent<Position>(e, Position{wave.x + i*50.f, wave.y});
            _ecs.addComponent<Velocity>(e, Velocity{-100.f, 0.f});
            _ecs.addComponent<Drawable>(e, Drawable{"enemy.png", {0,0,32,32}, 15});
            _ecs.addComponent<Collider>(e, Collider{32,32,true,2,10});
            _ecs.addComponent<Health>(e, Health{50,50});
        }
        _currentWave++;
        _waveTimer = (_currentWave < _waves.size()) ? _waves[_currentWave].time : 999.f;
    }
}