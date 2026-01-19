/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** PhysicsSystem - Gère la physique (gravité, forces, collisions)
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <algorithm>
#include <vector>

class PhysicsSystem : public ISystem {
public:
    explicit PhysicsSystem(ECS& ecs, float gravity = 980.f);
    void update(double dt) override;
    
    // Configuration de la physique
    void setGravity(float gravity) { _gravity = gravity; }
    float getGravity() const { return _gravity; }
    
    // Applique la gravité à une entité spécifique
    void applyGravity(Entity entity, float dt);
    
    // Applique une force à une entité
    void applyForce(Entity entity, sf::Vector2f force);
    
    // Applique une impulsion (force instantanée)
    void applyImpulse(Entity entity, sf::Vector2f impulse);
    
    // Vérifie si une entité est au sol
    bool isGrounded(Entity entity);
    
    // Effectue un raycast (pour vérifier le sol, par exemple)
    bool raycast(sf::Vector2f start, sf::Vector2f direction, float distance, Entity& hitEntity);

private:
    void updateRigidBodies(float dt);
    void updateJumpers(float dt);
    void updateVelocities(float dt);
    void resolveCollisions(float dt);
    
    // Obtient le rectangle de collision d'une entité
    sf::FloatRect getColliderRect(Entity entity);
    
    float _gravity;  // Gravité en pixels/s²
};

// ============= IMPLEMENTATION =============

PhysicsSystem::PhysicsSystem(ECS& ecs, float gravity)
    : ISystem(ecs), _gravity(gravity)
{
}

void PhysicsSystem::update(double dt)
{
    float fdt = static_cast<float>(dt);
    
    // 1. Applique la gravité et les forces
    updateRigidBodies(fdt);
    
    // 2. Met à jour les vélocités (drag, friction)
    updateVelocities(fdt);
    
    // 3. Résout les mouvements et collisions PAR AXE (X puis Y)
    resolveCollisions(fdt);
    
    // 4. Met à jour les jumpers (coyote time, jump buffer)
    updateJumpers(fdt);
}

void PhysicsSystem::updateRigidBodies(float dt)
{
    auto entities = _ecs.getEntitiesByComponents<RigidBody_t, Velocity_t>();
    
    for (Entity e : entities) {
        auto* rb = _ecs.getComponent<RigidBody_t>(e);
        auto* vel = _ecs.getComponent<Velocity_t>(e);
        
        if (!rb || !vel || rb->isKinematic) continue;
        
        // Applique la gravité
        if (rb->useGravity) {
            vel->y += _gravity * rb->gravityScale * dt;
        }
        
        // Applique le drag (résistance de l'air)
        if (rb->drag > 0.f) {
            float dragFactor = 1.f - std::min(rb->drag * dt, 1.f);
            vel->x *= dragFactor;
            vel->y *= dragFactor;
        }
    }
}

void PhysicsSystem::updateVelocities(float dt)
{
    auto entities = _ecs.getEntitiesByComponents<Velocity_t>();
    
    for (Entity e : entities) {
        auto* vel = _ecs.getComponent<Velocity_t>(e);
        if (!vel) continue;
        
        // Applique la friction si l'entité est au sol
        auto* rb = _ecs.getComponent<RigidBody_t>(e);
        if (rb && rb->isGrounded) {
            // Trouve la plateforme sous l'entité
            auto platforms = _ecs.getEntitiesByComponents<Platform_t>();
            for (Entity p : platforms) {
                auto* platform = _ecs.getComponent<Platform_t>(p);
                if (!platform) continue;
                
                // Applique la friction de la plateforme
                vel->x *= (1.f - platform->friction * dt * 5.f);  // Facteur de friction
                
                // Si la plateforme bouge, transfère sa vélocité
                if (platform->velocity.x != 0.f || platform->velocity.y != 0.f) {
                    vel->x += platform->velocity.x * dt;
                    vel->y += platform->velocity.y * dt;
                }
                break;  // Une seule plateforme à la fois
            }
        }
    }
}

void PhysicsSystem::updateJumpers(float dt)
{
    auto entities = _ecs.getEntitiesByComponents<Jumper_t, RigidBody_t>();
    
    for (Entity e : entities) {
        auto* jumper = _ecs.getComponent<Jumper_t>(e);
        auto* rb = _ecs.getComponent<RigidBody_t>(e);
        
        if (!jumper || !rb) continue;
        
        // Met à jour le coyote time
        if (rb->isGrounded) {
            jumper->coyoteCounter = jumper->coyoteTime;
            jumper->currentJumps = 0;  // Réinitialise les sauts
        } else {
            jumper->coyoteCounter -= dt;
        }
        
        // Met à jour le jump buffer
        if (jumper->jumpBufferCounter > 0.f) {
            jumper->jumpBufferCounter -= dt;
        }
        
        // Détermine si on peut sauter
        jumper->canJump = (jumper->coyoteCounter > 0.f || jumper->currentJumps < jumper->maxJumps);
    }
}

void PhysicsSystem::resolveCollisions(float dt)
{
    // Réinitialise l'état au sol pour toutes les entités
    auto rigidBodies = _ecs.getEntitiesByComponents<RigidBody_t>();
    for (Entity e : rigidBodies) {
        auto* rb = _ecs.getComponent<RigidBody_t>(e);
        if (rb) rb->isGrounded = false;
    }
    
    // Récupère toutes les entités dynamiques (qui bougent)
    auto dynamicEntities = _ecs.getEntitiesByComponents<Position_t, Velocity_t, BoxCollider_t, RigidBody_t>();
    
    // Récupère toutes les entités avec des colliders (plateformes incluses)
    auto staticEntities = _ecs.getEntitiesByComponents<Position_t, BoxCollider_t>();
    
    // Pour chaque entité dynamique
    for (Entity entity : dynamicEntities) {
        auto* pos = _ecs.getComponent<Position_t>(entity);
        auto* vel = _ecs.getComponent<Velocity_t>(entity);
        auto* col = _ecs.getComponent<BoxCollider_t>(entity);
        auto* rb = _ecs.getComponent<RigidBody_t>(entity);
        
        if (!pos || !vel || !col || !rb || rb->isKinematic) continue;
        
        // Calcule le déplacement souhaité (utilise le dt réel, pas hardcodé!)
        float moveX = vel->x * dt;
        float moveY = vel->y * dt;
        
        // === MOUVEMENT EN X (horizontal) ===
        if (moveX != 0.f) {
            pos->x += moveX;
            
            // Vérifie les collisions après le mouvement en X
            sf::FloatRect entityRect = getColliderRect(entity);
            
            for (Entity obstacle : staticEntities) {
                if (obstacle == entity) continue;
                
                auto* obsCol = _ecs.getComponent<BoxCollider_t>(obstacle);
                if (!obsCol || obsCol->isTrigger) continue;
                
                // Vérifie les layers de collision
                if ((col->collisionMask & (1 << obsCol->layer)) == 0) continue;
                
                sf::FloatRect obstacleRect = getColliderRect(obstacle);
                
                // Test de collision AABB
                if (entityRect.intersects(obstacleRect)) {
                    // Collision détectée ! Résout sur l'axe X
                    if (moveX > 0.f) {
                        // Bouge vers la droite → snap à gauche de l'obstacle
                        pos->x = obstacleRect.left - col->width - col->offsetX;
                    } else {
                        // Bouge vers la gauche → snap à droite de l'obstacle
                        pos->x = obstacleRect.left + obstacleRect.width - col->offsetX;
                    }
                    
                    // Annule la vélocité horizontale
                    vel->x = 0.f;
                    break;  // Une seule collision par axe
                }
            }
        }
        
        // === MOUVEMENT EN Y (vertical) ===
        if (moveY != 0.f) {
            pos->y += moveY;
            
            // Vérifie les collisions après le mouvement en Y
            sf::FloatRect entityRect = getColliderRect(entity);
            
            for (Entity obstacle : staticEntities) {
                if (obstacle == entity) continue;
                
                auto* obsCol = _ecs.getComponent<BoxCollider_t>(obstacle);
                if (!obsCol || obsCol->isTrigger) continue;
                
                // Vérifie les layers de collision
                if ((col->collisionMask & (1 << obsCol->layer)) == 0) continue;
                
                sf::FloatRect obstacleRect = getColliderRect(obstacle);
                
                // Test de collision AABB
                if (entityRect.intersects(obstacleRect)) {
                    // Collision détectée ! Résout sur l'axe Y
                    if (moveY > 0.f) {
                        // Tombe vers le bas → snap au-dessus de l'obstacle (atterrissage)
                        pos->y = obstacleRect.top - col->height - col->offsetY;
                        rb->isGrounded = true;
                        
                        // Applique le rebond ou annule la vélocité
                        if (rb->bounciness > 0.f) {
                            vel->y = -vel->y * rb->bounciness;
                        } else {
                            vel->y = 0.f;
                        }
                    } else {
                        // Monte vers le haut → snap en-dessous de l'obstacle (frappe le plafond)
                        pos->y = obstacleRect.top + obstacleRect.height - col->offsetY;
                        vel->y = 0.f;
                    }
                    break;  // Une seule collision par axe
                }
            }
        }
        
        // === VÉRIFICATION SUPPLÉMENTAIRE DU SOL (pour éviter les faux positifs) ===
        // Vérifie si l'entité est juste au-dessus d'une plateforme (distance < 2 pixels)
        if (!rb->isGrounded && vel->y >= 0.f) {
            sf::FloatRect entityRect = getColliderRect(entity);
            entityRect.top += 2.f;  // Étend légèrement vers le bas
            
            for (Entity obstacle : staticEntities) {
                if (obstacle == entity) continue;
                
                auto* obsCol = _ecs.getComponent<BoxCollider_t>(obstacle);
                if (!obsCol || obsCol->isTrigger) continue;
                
                if ((col->collisionMask & (1 << obsCol->layer)) == 0) continue;
                
                sf::FloatRect obstacleRect = getColliderRect(obstacle);
                
                if (entityRect.intersects(obstacleRect)) {
                    rb->isGrounded = true;
                    break;
                }
            }
        }
    }
}

sf::FloatRect PhysicsSystem::getColliderRect(Entity entity)
{
    auto* pos = _ecs.getComponent<Position_t>(entity);
    auto* col = _ecs.getComponent<BoxCollider_t>(entity);
    
    if (!pos || !col) return sf::FloatRect();
    
    return sf::FloatRect(
        pos->x + col->offsetX,
        pos->y + col->offsetY,
        col->width,
        col->height
    );
}

void PhysicsSystem::applyGravity(Entity entity, float dt)
{
    auto* rb = _ecs.getComponent<RigidBody_t>(entity);
    auto* vel = _ecs.getComponent<Velocity_t>(entity);
    
    if (rb && vel && rb->useGravity && !rb->isKinematic) {
        vel->y += _gravity * rb->gravityScale * dt;
    }
}

void PhysicsSystem::applyForce(Entity entity, sf::Vector2f force)
{
    auto* rb = _ecs.getComponent<RigidBody_t>(entity);
    auto* vel = _ecs.getComponent<Velocity_t>(entity);
    
    if (rb && vel && !rb->isKinematic) {
        // F = ma => a = F/m => v += a*dt (intégré dans le prochain update)
        vel->x += force.x / rb->mass;
        vel->y += force.y / rb->mass;
    }
}

void PhysicsSystem::applyImpulse(Entity entity, sf::Vector2f impulse)
{
    auto* rb = _ecs.getComponent<RigidBody_t>(entity);
    auto* vel = _ecs.getComponent<Velocity_t>(entity);
    
    if (rb && vel && !rb->isKinematic) {
        // Impulsion = changement instantané de vélocité
        vel->x += impulse.x / rb->mass;
        vel->y += impulse.y / rb->mass;
    }
}

bool PhysicsSystem::isGrounded(Entity entity)
{
    auto* rb = _ecs.getComponent<RigidBody_t>(entity);
    return rb && rb->isGrounded;
}

bool PhysicsSystem::raycast(sf::Vector2f start, sf::Vector2f direction, float distance, Entity& hitEntity)
{
    // Normalise la direction
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length == 0.f) return false;
    
    direction.x /= length;
    direction.y /= length;
    
    sf::Vector2f end = start + direction * distance;
    
    // Vérifie l'intersection avec tous les colliders
    auto entities = _ecs.getEntitiesByComponents<Position_t, BoxCollider_t>();
    
    float closestDist = distance;
    Entity closest = static_cast<Entity>(-1);
    
    for (Entity e : entities) {
        sf::FloatRect rect = getColliderRect(e);
        
        // Test d'intersection rayon-rectangle simplifié
        // (pour une vraie implémentation, utiliser un algo plus robuste)
        if (rect.contains(end)) {
            float dist = std::sqrt((end.x - start.x) * (end.x - start.x) + 
                                   (end.y - start.y) * (end.y - start.y));
            if (dist < closestDist) {
                closestDist = dist;
                closest = e;
            }
        }
    }
    
    if (closest != static_cast<Entity>(-1)) {
        hitEntity = closest;
        return true;
    }
    
    return false;
}
