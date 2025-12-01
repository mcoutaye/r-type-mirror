/*
** EPITECH PROJECT, 2025
** G-CPP-500-RUN-5-2-rtype-3
** File description:
** ecs
*/

#include "ecs.hpp"
#include "tcp.hpp"
#include "udp.hpp"
#include <iostream>

// Those will be componentstype used in the ECS
typedef struct position_s {
    float x;
    float y;
} position_t;

typedef struct velocity_s {
    float x;
    float y;
} velocity_t;

// Make a system for tests
class MovementSystem : public ISystem {
    public:

        MovementSystem(ECS &ecs)
            : ISystem(ecs) {}

        ~MovementSystem() override = default;

        void update(double dt) override
        {

            std::vector<Entity> entities = _ecs.getEntitiesByComponents<position_t, velocity_t>();
            std::size_t big = entities.size();

            for (Entity i = 0; i < big; i ++) {

                auto entity = entities[i];
                auto pos = _ecs.getComponent<position_t>(entity);
                auto velo = _ecs.getComponent<velocity_t>(entity);

                if (!pos || !velo) {
                // THIS SHOULD NOT TRIGGER THANKS TO getEntitiesByComponent AND Signatures
                    std::cerr << "Something went wrong in Movement System on Entity " << entity << std::endl;
                    continue;
                }

                pos->x += velo->x * dt;
                pos->y += velo->y * dt;
            }
        }
};


int main(int ac, char **av)
{
    // Init the ECS
    ECS ecs;

    // Init the TCP Server for heartbeat
    // Server TCP(8000);

    // if (!TCP.init()) {
    //     std::cerr << "Failed to init TCP Server." << std::endl;
    //     return 84;
    // }

    // Init the systems
    MovementSystem movementSystem(ecs);

    // Start server
    // TCP.start();

    // Create 2 entites
    Entity e1 = ecs.createEntity();
    Entity e2 = ecs.createEntity();
    Entity e3 = ecs.createEntity();

    std::cout << "Created entities: " << e1 << " and " << e2 << std::endl;

    // Create Components
    ecs.addComponent<position_t>(e1, position_t{1, 2});
    ecs.addComponent<velocity_t>(e1, velocity_t{0.54, -3.2});

    ecs.addComponent<position_t>(e2, position_t{5, 6});

    ecs.addComponents(e3, position_t{12, 14.5}, velocity_t{0.54, -3.2});
    std::cout << "Created Entity " << e3 << "= velocity (struct):" << ecs.getConstComponent<velocity_t>(e3)
        << " | position: " << ecs.getConstComponent<position_t>(e3)->x << ";" << ecs.getConstComponent<position_t>(e3)->y << std::endl;

    // Find component ?
    std::cout << "Entity " << e1 << " has Position? "
        << ecs.hasComponent<position_t>(e1) << std::endl;
    std::cout << "Entity " << e1 << " has Velocity? "
        << ecs.hasComponent<velocity_t>(e1) << std::endl;

    std::cout << "Entity " << e2 << " has Position? "
        << ecs.hasComponent<position_t>(e2) << std::endl;
    std::cout << "Entity " << e2 << " has Velocity? "
        << ecs.hasComponent<velocity_t>(e2) << std::endl;


    // Update
    // auto *position = ecs.getComponent<position_t>(e1);
    // auto *velocity = ecs.getComponent<velocity_t>(e1);

    // std::cout << "Before Update, Entity " << e1 << " is at {" << position->x << ":" << position->y << "}" << std::endl;

    // position->x += velocity->x;
    // position->y += velocity->y;

    // std::cout << "After Update, Entity " << e1 << " is at {" << new_pos->x << ":" << new_pos->y << "}" << std::endl;


    // Update with Systems
    auto *pos = ecs.getConstComponent<position_t>(e1);
    std::cout << "Before Update, Entity " << e1 << " is at {" << pos->x << ", " << pos->y << "}." << std::endl;

    movementSystem.update(1.0f); // Update one second after

    std::cout << "After Update, Entity " << e1 << " is at {" << pos->x << ", " << pos->y << "}." << std::endl;


    // Exemple when Entity has no such component
    auto *test_velo = ecs.getConstComponent<velocity_t>(e2);
    if (test_velo == nullptr)
        std::cout << "Entity " << e2 << " has no Velocity component." << std::endl;
    else
        std::cout << "Test Velocity, Entity " << e1 << " is at " << test_velo->x << ", " << test_velo->y << std::endl;


    // Test destroying Entity
    auto *new_pos = ecs.getConstComponent<position_t>(e1);

    ecs.killEntity(e1);
    std::cout << "Destroyed Entity " << e1 << "..." << std::endl;

    if (ecs.hasComponent<position_t>(e1))
        std::cout << "Failed to destroyed, Entity " << e1 << " is at {" << new_pos->x << ":" << new_pos->y << "}" << std::endl;
    else
        std::cout << "Successfully." << std::endl;

    ecs.killEntity(e2);
    ecs.killEntity(e3);

    // std::this_thread::sleep_for(std::chrono::seconds(20));

    // TCP.stop();
    // TCP.join();

    // Server UDP
    UDP udpServer(8080);

    if (!udpServer.init()) {
        std::cerr << "Failed to init UDP Server." << std::endl;
        return 84;
    }
    udpServer.run();

    return 0;
}
