#include "../../../include/engine/systems/MissileSystem.hpp"
#include <iostream>

void MissileSystem::update(double dt) {
    auto shootableEntities = _ecs.getEntitiesByComponents<Shootable, Position>();
    for (Entity e : shootableEntities) {
        auto* shootable = _ecs.getComponent<Shootable>(e);
        auto* pos = _ecs.getComponent<Position>(e);

        bool canShoot = false;

        // Si c'est le joueur, il faut que isShooting soit vrai
        if (_ecs.hasComponent<PlayerController>(e)) {
            canShoot = shootable->isShooting && shootable->cooldown <= 0.f;
        }
        // Si c'est un ennemi, il tire automatiquement (cooldown écoulé)
        else {
            canShoot = shootable->cooldown <= 0.f;
        }

        if (canShoot) {
            // Crée un projectile
            Entity missile = _ecs.createEntity();
            _ecs.addComponent(missile, Position{pos->x + shootable->offsetX, pos->y + shootable->offsetY});
            _ecs.addComponent(missile, Velocity{shootable->missileSpeed, 0.f});
            _ecs.addComponent(missile, Drawable{shootable->textureId, {0, 0, 16, 8}, 20, true});
            _ecs.addComponent(missile, Collider{16.f, 8.f, false, shootable->team, shootable->damage});
            _ecs.addComponent(missile, Projectile{shootable->missileSpeed, shootable->damage});

            // Réinitialise le cooldown
            shootable->cooldown = shootable->shootDelay;
        } else {
            // Décrémente le cooldown
            shootable->cooldown -= dt;
        }

        // Réinitialise isShooting pour le joueur uniquement
        if (_ecs.hasComponent<PlayerController>(e)) {
            shootable->isShooting = false;
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
