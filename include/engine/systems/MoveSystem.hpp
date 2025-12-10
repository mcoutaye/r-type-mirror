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

            for (Entity e : entities) {

                auto pos = _ecs.getComponent<Position_t>(e);
                auto velo = _ecs.getComponent<Velocity_t>(e);

                if (!pos || !velo) {
                // THIS SHOULD NOT TRIGGER THANKS TO getEntitiesByComponent AND Signatures
                    std::cerr << "Something went wrong in Movement System on Entity " << e << std::endl;
                    continue;
                }

                pos->x += velo->x * dt;
                pos->y += velo->y * dt;

                // Velocity should not reset here, better in game logic systems
                velo->x = 0;
                velo->y = 0;
            }
        }
};


#endif /* !MOVESYSTEM_HPP_ */