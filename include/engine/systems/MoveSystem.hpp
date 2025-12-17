/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** MoveSystem
*/

#ifndef MOVESYSTEM_HPP_
    #define MOVESYSTEM_HPP_

    #include "ecs.hpp"
    #include "engine/systems/Components.hpp"

class MoveSystem : public ISystem {
    public:

        MoveSystem(ECS &ecs)
            : ISystem(ecs) {}

        ~MoveSystem() override = default;

        void update(double dt) override
        {
            std::vector<Entity> entities = _ecs.getEntitiesByComponents<Position_t, Velocity_t>();
            std::vector<Entity> obstacles = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

            for (Entity e : entities) {

                auto pos = _ecs.getComponent<Position_t>(e);
                auto velo = _ecs.getComponent<Velocity_t>(e);

                if (!pos || !velo) {
                // THIS SHOULD NOT TRIGGER THANKS TO getEntitiesByComponent AND Signatures
                    std::cerr << "Something went wrong in Movement System on Entity " << e << std::endl;
                    continue;
                }

                auto collider = _ecs.getComponent<Collider_t>(e);

                if (collider) {
                    // Check X axis
                    float nextX = pos->x + velo->x * dt;
                    bool collisionX = false;

                    for (Entity obs : obstacles) {
                        if (e == obs) continue;
                        auto obsCollider = _ecs.getComponent<Collider_t>(obs);
                        if (!obsCollider || !obsCollider->solid) continue;
                        if (obsCollider->team != 0) continue; // Only collide with environment (team 0)

                        auto obsPos = _ecs.getComponent<Position_t>(obs);
                        if (!obsPos) continue;

                        if (nextX < obsPos->x + obsCollider->width &&
                            nextX + collider->width > obsPos->x &&
                            pos->y < obsPos->y + obsCollider->height &&
                            pos->y + collider->height > obsPos->y) {

                            collisionX = true;
                            // Resolve collision
                            if (velo->x > 0) {
                                pos->x = obsPos->x - collider->width;
                            } else if (velo->x < 0) {
                                pos->x = obsPos->x + obsCollider->width;
                            } else {
                                float overlapLeft = (pos->x + collider->width) - obsPos->x;
                                float overlapRight = (obsPos->x + obsCollider->width) - pos->x;
                                if (overlapLeft < overlapRight) pos->x = obsPos->x - collider->width;
                                else pos->x = obsPos->x + obsCollider->width;
                            }
                            velo->x = 0;
                            break;
                        }
                    }
                    if (!collisionX) pos->x = nextX;

                    // Check Y axis
                    float nextY = pos->y + velo->y * dt;
                    bool collisionY = false;

                    for (Entity obs : obstacles) {
                        if (e == obs) continue;
                        auto obsCollider = _ecs.getComponent<Collider_t>(obs);
                        if (!obsCollider || !obsCollider->solid) continue;
                        if (obsCollider->team != 0) continue; // Only collide with environment (team 0)

                        auto obsPos = _ecs.getComponent<Position_t>(obs);
                        if (!obsPos) continue;

                        if (pos->x < obsPos->x + obsCollider->width &&
                            pos->x + collider->width > obsPos->x &&
                            nextY < obsPos->y + obsCollider->height &&
                            nextY + collider->height > obsPos->y) {

                            collisionY = true;
                            // Resolve collision
                            if (velo->y > 0) {
                                pos->y = obsPos->y - collider->height;
                            } else if (velo->y < 0) {
                                pos->y = obsPos->y + obsCollider->height;
                            } else {
                                float overlapTop = (pos->y + collider->height) - obsPos->y;
                                float overlapBottom = (obsPos->y + obsCollider->height) - pos->y;
                                if (overlapTop < overlapBottom) pos->y = obsPos->y - collider->height;
                                else pos->y = obsPos->y + obsCollider->height;
                            }
                            velo->y = 0;
                            break;
                        }
                    }
                    if (!collisionY) pos->y = nextY;

                } else {
                    pos->x += velo->x * dt;
                    pos->y += velo->y * dt;
                }

                if (_ecs.hasComponent<PlayerController_t>(e)) {
                    velo->x = 0;
                    velo->y = 0;
                }
                // Velocity should not reset here, better in game logic systems
                // velo->x = 0;
                // velo->y = 0;
            }


        }
};

#endif /* !MOVESYSTEM_HPP_ */