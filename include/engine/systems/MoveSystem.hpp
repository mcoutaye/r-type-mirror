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
            std::vector<Entity> tiles = _ecs.getEntitiesByComponents<Position_t, Collider_t, DestructibleTile_t>();

            for (Entity e : entities) {

                auto pos = _ecs.getComponent<Position_t>(e);
                auto velo = _ecs.getComponent<Velocity_t>(e);
                auto col = _ecs.getComponent<Collider_t>(e);
                auto projectile = _ecs.getComponent<Projectile_t>(e);

                if (!pos || !velo) {
                // THIS SHOULD NOT TRIGGER THANKS TO getEntitiesByComponent AND Signatures
                    std::cerr << "Something went wrong in Movement System on Entity " << e << std::endl;
                    continue;
                }

                // Calculate new position
                float newX = pos->x + velo->x * dt;
                float newY = pos->y + velo->y * dt;

                // Check collision with destructible tiles (only for non-projectile entities with collider)
                bool blockedX = false;
                bool blockedY = false;
                
                if (col && !projectile) {
                    // Projectiles pass through tiles, only non-projectiles are blocked
                    for (Entity tile : tiles) {
                        auto tilePos = _ecs.getComponent<Position_t>(tile);
                        auto tileCol = _ecs.getComponent<Collider_t>(tile);
                        auto tileDef = _ecs.getComponent<DestructibleTile_t>(tile);

                        if (!tilePos || !tileCol || !tileDef || !tileDef->blocksMovement)
                            continue;

                        // Check X-axis collision
                        Position_t testPosX = {newX, pos->y};
                        if (checkAABB(&testPosX, col, tilePos, tileCol)) {
                            blockedX = true;
                        }

                        // Check Y-axis collision
                        Position_t testPosY = {pos->x, newY};
                        if (checkAABB(&testPosY, col, tilePos, tileCol)) {
                            blockedY = true;
                        }
                    }
                }

                // Apply movement (blocked axes don't move)
                if (!blockedX) {
                    pos->x = newX;
                }
                if (!blockedY) {
                    pos->y = newY;
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
    private:
        bool checkAABB(Position_t* pos1, Collider_t* col1, 
                   Position_t* pos2, Collider_t* col2) const
    {
        float left1 = pos1->x - col1->width / 2.f;
        float right1 = pos1->x + col1->width / 2.f;
        float top1 = pos1->y - col1->height / 2.f;
        float bottom1 = pos1->y + col1->height / 2.f;

        float left2 = pos2->x - col2->width / 2.f;
        float right2 = pos2->x + col2->width / 2.f;
        float top2 = pos2->y - col2->height / 2.f;
        float bottom2 = pos2->y + col2->height / 2.f;

        return (left1 < right2 && right1 > left2 && 
                top1 < bottom2 && bottom1 > top2);
    }
};

#endif /* !MOVESYSTEM_HPP_ */