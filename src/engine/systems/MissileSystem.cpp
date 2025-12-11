#include "../../../include/engine/systems/MissileSystem.hpp"
#include <iostream>
#include "../../../include/engine/systems/EntityFactory.hpp"

void MissileSystem::update(double dt) {
    auto shootableEntities = _ecs.getEntitiesByComponents<Shootable, Position>();
    for (Entity e : shootableEntities) {
        auto* shootable = _ecs.getComponent<Shootable>(e);
        auto* pos = _ecs.getComponent<Position>(e);

        // Décrémente le cooldown
        shootable->cooldown -= dt;

        // Vérifie si l'entité peut tirer
        bool canShoot = shootable->cooldown <= 0.f;
        if (_ecs.hasComponent<PlayerController>(e)) {
            canShoot = canShoot && shootable->isShooting;
        }

        if (canShoot) {
            // Triple shot si activé
            if (shootable->tripleShot && _ecs.hasComponent<PlayerController>(e)) {
                for (int i = -1; i <= 1; ++i) {
                    Entity missile = Factory::createProjectile(
                        _ecs,
                        pos->x + shootable->offsetX,
                        pos->y + shootable->offsetY,
                        shootable->missileSpeed,
                        i * 200.f,  // Angle légèrement différent
                        shootable->team,
                        shootable->damage,
                        shootable->textureId
                    );
                }
            } else {
                // Tir normal
                Factory::createProjectile(
                    _ecs,
                    pos->x + shootable->offsetX,
                    pos->y + shootable->offsetY,
                    shootable->missileSpeed,
                    0.f,
                    shootable->team,
                    shootable->damage,
                    shootable->textureId
                );
            }
            // Réinitialise le cooldown et isShooting
            shootable->cooldown = shootable->shootDelay;
            if (_ecs.hasComponent<PlayerController>(e)) {
                shootable->isShooting = false;
            }
        }
    }

    // Suppression des projectiles hors écran
    for (Entity e : _ecs.getEntitiesByComponents<Projectile, Position>()) {
        auto* pos = _ecs.getComponent<Position>(e);
        if (pos->x > 2000.f || pos->x < -100.f || pos->y > 1200.f || pos->y < -100.f) {
            _ecs.killEntity(e);
        }
    }
}
