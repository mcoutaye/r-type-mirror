/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include "UdpClient.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/Components.hpp"
#include "ecs.hpp"
#include <chrono>

using namespace std::chrono_literals;

void processUpdates(const EntityUpdate update, ECS& ecsClient)
{
    if (update.entityId >= MAX_ENTITIES)
        return;

    if (!ecsClient.hasComponent<Position_t>(update.entityId)) {
        ecsClient.addComponent<Position_t>(update.entityId, {update.position.x, update.position.y});
    } else {
        auto pos = ecsClient.getComponent<Position_t>(update.entityId);
        if (pos) {
            pos->x = update.position.x;
            pos->y = update.position.y;
        }
    }

    if (!ecsClient.hasComponent<Health_t>(update.entityId)) {
        ecsClient.addComponent<Health_t>(update.entityId, {update.health.max, update.health.current});
    } else {
        auto health = ecsClient.getComponent<Health_t>(update.entityId);
        if (health) {
            health->current = update.health.current;
            health->max = update.health.max;
        }
    }

    auto pos = ecsClient.getComponent<Position_t>(update.entityId);
    if (pos) {
        std::cout << "\tEntity " << update.entityId
            << " → {" << pos->x
            << ", " << pos->y
            << "}" << std::endl;
    }
}

int main()
{
    UdpClient client("10.49.84.103", 53000);
    ECS ecsClient;
    InputSystem inSys(ecsClient);
    if (!client.start())
        return -1;
    Entity test = ecsClient.createEntity();
    ecsClient.addComponents<Position_t, Health_t, Velocity_t>(test, {0.f, 0.f}, {100, 100}, {0.f, 0.f});
    while (true) {
        InputState inputs = {0, 0, 0, 0, 0, 0};
        inSys.update(0);
        if (inSys.isActionActive(GameAction::MoveUp))
            inputs.up = 1;
        if (inSys.isActionActive(GameAction::MoveDown))
            inputs.down = 1;
        if (inSys.isActionActive(GameAction::MoveLeft))
            inputs.left = 1;
        if (inSys.isActionActive(GameAction::MoveRight))
            inputs.right = 1;
        if (inSys.isActionActive(GameAction::Shoot))
            inputs.shoot = 1;
        client.inputsToSend.push(inputs);

        std::vector<EntityUpdate> updates;
        std::vector<EntityUpdate> first;

        if (!client.receivedUpdates.tryPop(updates))
            continue;

        first = updates;

        while (first[0].tick <= updates[0].tick) {
            for (const auto& e : updates) {
                processUpdates(e, ecsClient);
            }
            if (!client.receivedUpdates.tryPop(updates))
                break;
        }

        for (const auto& e : updates) {
            processUpdates(e, ecsClient);
        }

        std::this_thread::sleep_for(100ms);
    }
    client.stop();
    client.join();
    return 0;
}
