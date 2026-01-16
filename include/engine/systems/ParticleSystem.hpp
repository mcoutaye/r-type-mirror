/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** ParticleSystem - Système de particules avec pool
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>
#include <cmath>
#include <random>

class ParticleSystem : public ISystem {
public:
    explicit ParticleSystem(ECS& ecs, size_t poolSize = 2000);
    void update(double dt) override;
    void render(sf::RenderWindow& window);
    
    // Émet un burst de particules à une position
    void emit(ParticleType type, float x, float y, int count = 20);
    
    // Émet des particules avec des paramètres personnalisés
    void emitCustom(float x, float y, int count, float speed, float spread,
                    float lifetime, sf::Color startColor, sf::Color endColor,
                    float startSize, float endSize, float gravity = 0.f);
    
    // Presets
    void emitExplosion(float x, float y, int count = 30);
    void emitSparks(float x, float y, int count = 15);
    void emitSmoke(float x, float y, int count = 10);
    void emitDebris(float x, float y, int count = 8);
    void emitTrail(float x, float y, float directionDeg, int count = 3);

private:
    Particle_t* getParticle();  // Récupère une particule libre du pool
    void updateParticle(Particle_t& p, float dt);
    void spawnParticle(Particle_t& p, float x, float y, const ParticleEmitter_t& emitter);
    sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t);
    float randomFloat(float min, float max);
    
    std::vector<Particle_t> _pool;
    size_t _poolSize;
    sf::CircleShape _circleShape;
    
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist{0.f, 1.f};
};

// ============= IMPLEMENTATION =============

ParticleSystem::ParticleSystem(ECS& ecs, size_t poolSize)
    : ISystem(ecs), _poolSize(poolSize), _rng(std::random_device{}())
{
    _pool.resize(poolSize);
    _circleShape.setPointCount(8);  // Octogone pour perf
}

void ParticleSystem::update(double dt)
{
    float fdt = static_cast<float>(dt);
    
    // Met à jour les particules actives
    for (auto& p : _pool) {
        if (p.active) {
            updateParticle(p, fdt);
        }
    }
    
    // Gère les émetteurs attachés aux entités
    auto emitters = _ecs.getEntitiesByComponents<ParticleEmitter_t, Position_t>();
    
    for (Entity e : emitters) {
        auto* emitter = _ecs.getComponent<ParticleEmitter_t>(e);
        auto* pos = _ecs.getComponent<Position_t>(e);
        
        if (!emitter || !pos || !emitter->active) continue;
        
        // Gère la durée de vie de l'émetteur
        if (emitter->emitterLifetime > 0.f) {
            emitter->emitterLifetime -= fdt;
            if (emitter->emitterLifetime <= 0.f) {
                emitter->active = false;
                continue;
            }
        }
        
        if (emitter->burst) {
            // Mode burst : émet tout d'un coup
            for (int i = 0; i < emitter->particlesPerBurst; ++i) {
                Particle_t* p = getParticle();
                if (p) spawnParticle(*p, pos->x, pos->y, *emitter);
            }
            emitter->burst = false;  // Désactive après le burst
        } else {
            // Mode continu : accumule et spawn selon spawnRate
            emitter->spawnAccumulator += emitter->spawnRate * fdt;
            
            while (emitter->spawnAccumulator >= 1.f) {
                Particle_t* p = getParticle();
                if (p) spawnParticle(*p, pos->x, pos->y, *emitter);
                emitter->spawnAccumulator -= 1.f;
            }
        }
    }
}

void ParticleSystem::render(sf::RenderWindow& window)
{
    for (const auto& p : _pool) {
        if (!p.active) continue;
        
        // Interpole la couleur selon le temps de vie
        float t = 1.f - (p.lifetime / p.maxLifetime);
        sf::Color color = lerpColor(p.startColor, p.endColor, t);
        
        // Interpole la taille
        float size = p.startSize + (p.endSize - p.startSize) * t;
        
        _circleShape.setRadius(size / 2.f);
        _circleShape.setOrigin(size / 2.f, size / 2.f);
        _circleShape.setPosition(p.x, p.y);
        _circleShape.setFillColor(color);
        _circleShape.setRotation(p.rotation);
        
        window.draw(_circleShape);
    }
}

Particle_t* ParticleSystem::getParticle()
{
    for (auto& p : _pool) {
        if (!p.active) {
            return &p;
        }
    }
    return nullptr;  // Pool plein
}

void ParticleSystem::updateParticle(Particle_t& p, float dt)
{
    p.lifetime -= dt;
    
    if (p.lifetime <= 0.f) {
        p.active = false;
        return;
    }
    
    // Applique l'accélération
    p.vx += p.ax * dt;
    p.vy += p.ay * dt;
    
    // Applique la vélocité
    p.x += p.vx * dt;
    p.y += p.vy * dt;
    
    // Applique la rotation
    p.rotation += p.rotationSpeed * dt;
}

void ParticleSystem::spawnParticle(Particle_t& p, float x, float y, const ParticleEmitter_t& emitter)
{
    p.active = true;
    p.x = x;
    p.y = y;
    p.lifetime = emitter.particleLifetime * randomFloat(0.8f, 1.2f);
    p.maxLifetime = p.lifetime;
    
    // Direction aléatoire dans le spread
    float angle = emitter.direction + randomFloat(-emitter.spread / 2.f, emitter.spread / 2.f);
    float rad = angle * 3.14159f / 180.f;
    float speed = emitter.speed * randomFloat(0.7f, 1.3f);
    
    p.vx = std::cos(rad) * speed;
    p.vy = std::sin(rad) * speed;
    
    p.ax = 0.f;
    p.ay = emitter.gravity;
    
    p.startSize = emitter.startSize * randomFloat(0.8f, 1.2f);
    p.endSize = emitter.endSize;
    p.size = p.startSize;
    
    p.startColor = emitter.startColor;
    p.endColor = emitter.endColor;
    
    p.rotation = randomFloat(0.f, 360.f);
    p.rotationSpeed = randomFloat(-180.f, 180.f);
}

sf::Color ParticleSystem::lerpColor(const sf::Color& a, const sf::Color& b, float t)
{
    return sf::Color(
        static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
        static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
        static_cast<sf::Uint8>(a.b + (b.b - a.b) * t),
        static_cast<sf::Uint8>(a.a + (b.a - a.a) * t)
    );
}

float ParticleSystem::randomFloat(float min, float max)
{
    return min + _dist(_rng) * (max - min);
}

// ============= EMIT FUNCTIONS =============

void ParticleSystem::emit(ParticleType type, float x, float y, int count)
{
    switch (type) {
        case ParticleType::Explosion: emitExplosion(x, y, count); break;
        case ParticleType::Sparks:    emitSparks(x, y, count); break;
        case ParticleType::Smoke:     emitSmoke(x, y, count); break;
        case ParticleType::Debris:    emitDebris(x, y, count); break;
        case ParticleType::Trail:     emitTrail(x, y, 0.f, count); break;
    }
}

void ParticleSystem::emitCustom(float x, float y, int count, float speed, float spread,
                                 float lifetime, sf::Color startColor, sf::Color endColor,
                                 float startSize, float endSize, float gravity)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x;
        p->y = y;
        p->lifetime = lifetime * randomFloat(0.8f, 1.2f);
        p->maxLifetime = p->lifetime;
        
        float angle = randomFloat(-spread / 2.f, spread / 2.f);
        float rad = angle * 3.14159f / 180.f;
        float spd = speed * randomFloat(0.7f, 1.3f);
        
        p->vx = std::cos(rad) * spd;
        p->vy = std::sin(rad) * spd;
        p->ax = 0.f;
        p->ay = gravity;
        
        p->startSize = startSize * randomFloat(0.8f, 1.2f);
        p->endSize = endSize;
        p->startColor = startColor;
        p->endColor = endColor;
        
        p->rotation = randomFloat(0.f, 360.f);
        p->rotationSpeed = randomFloat(-180.f, 180.f);
    }
}

void ParticleSystem::emitExplosion(float x, float y, int count)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x;
        p->y = y;
        p->lifetime = randomFloat(0.3f, 0.8f);
        p->maxLifetime = p->lifetime;
        
        // Direction aléatoire (360°)
        float angle = randomFloat(0.f, 360.f) * 3.14159f / 180.f;
        float speed = randomFloat(100.f, 300.f);
        
        p->vx = std::cos(angle) * speed;
        p->vy = std::sin(angle) * speed;
        p->ax = 0.f;
        p->ay = 50.f;  // Légère gravité
        
        p->startSize = randomFloat(4.f, 10.f);
        p->endSize = 0.f;
        p->startColor = sf::Color(255, randomFloat(100, 200), 0);
        p->endColor = sf::Color(100, 0, 0, 0);
        
        p->rotation = 0.f;
        p->rotationSpeed = randomFloat(-200.f, 200.f);
    }
}

void ParticleSystem::emitSparks(float x, float y, int count)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x;
        p->y = y;
        p->lifetime = randomFloat(0.2f, 0.5f);
        p->maxLifetime = p->lifetime;
        
        float angle = randomFloat(0.f, 360.f) * 3.14159f / 180.f;
        float speed = randomFloat(200.f, 400.f);
        
        p->vx = std::cos(angle) * speed;
        p->vy = std::sin(angle) * speed;
        p->ax = 0.f;
        p->ay = 300.f;  // Forte gravité
        
        p->startSize = randomFloat(2.f, 4.f);
        p->endSize = 0.f;
        p->startColor = sf::Color::Yellow;
        p->endColor = sf::Color(255, 100, 0, 0);
        
        p->rotation = 0.f;
        p->rotationSpeed = 0.f;
    }
}

void ParticleSystem::emitSmoke(float x, float y, int count)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x + randomFloat(-10.f, 10.f);
        p->y = y;
        p->lifetime = randomFloat(1.f, 2.f);
        p->maxLifetime = p->lifetime;
        
        // Monte vers le haut avec léger mouvement horizontal
        p->vx = randomFloat(-20.f, 20.f);
        p->vy = randomFloat(-80.f, -40.f);  // Négatif = vers le haut
        p->ax = randomFloat(-10.f, 10.f);
        p->ay = -20.f;  // Continue à monter
        
        p->startSize = randomFloat(8.f, 15.f);
        p->endSize = randomFloat(20.f, 30.f);  // Grossit
        p->startColor = sf::Color(100, 100, 100, 200);
        p->endColor = sf::Color(50, 50, 50, 0);
        
        p->rotation = randomFloat(0.f, 360.f);
        p->rotationSpeed = randomFloat(-30.f, 30.f);
    }
}

void ParticleSystem::emitDebris(float x, float y, int count)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x;
        p->y = y;
        p->lifetime = randomFloat(0.8f, 1.5f);
        p->maxLifetime = p->lifetime;
        
        float angle = randomFloat(-60.f, 60.f) - 90.f;  // Vers le haut principalement
        float rad = angle * 3.14159f / 180.f;
        float speed = randomFloat(150.f, 300.f);
        
        p->vx = std::cos(rad) * speed;
        p->vy = std::sin(rad) * speed;
        p->ax = 0.f;
        p->ay = 500.f;  // Forte gravité
        
        p->startSize = randomFloat(6.f, 12.f);
        p->endSize = randomFloat(4.f, 8.f);
        p->startColor = sf::Color(80, 80, 80);
        p->endColor = sf::Color(40, 40, 40, 100);
        
        p->rotation = randomFloat(0.f, 360.f);
        p->rotationSpeed = randomFloat(-300.f, 300.f);
    }
}

void ParticleSystem::emitTrail(float x, float y, float directionDeg, int count)
{
    for (int i = 0; i < count; ++i) {
        Particle_t* p = getParticle();
        if (!p) return;
        
        p->active = true;
        p->x = x + randomFloat(-5.f, 5.f);
        p->y = y + randomFloat(-5.f, 5.f);
        p->lifetime = randomFloat(0.1f, 0.3f);
        p->maxLifetime = p->lifetime;
        
        // Opposé à la direction de déplacement
        float angle = (directionDeg + 180.f + randomFloat(-20.f, 20.f)) * 3.14159f / 180.f;
        float speed = randomFloat(30.f, 80.f);
        
        p->vx = std::cos(angle) * speed;
        p->vy = std::sin(angle) * speed;
        p->ax = 0.f;
        p->ay = 0.f;
        
        p->startSize = randomFloat(3.f, 6.f);
        p->endSize = 0.f;
        p->startColor = sf::Color(100, 150, 255, 200);
        p->endColor = sf::Color(50, 100, 200, 0);
        
        p->rotation = 0.f;
        p->rotationSpeed = 0.f;
    }
}
