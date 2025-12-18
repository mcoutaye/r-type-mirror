/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Input System
*/

#pragma once

#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <map>

enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Quit
};

class InputSystem : public ISystem {
public:
    InputSystem(ECS& ecs);
    ~InputSystem() = default;

    void update(double dt) override;

    bool isActionActive(GameAction action) const;
    bool wasActionPressed(GameAction action) const;
    bool wasActionReleased(GameAction action) const;
    void setKeyMapping(sf::Keyboard::Key key, GameAction action);
    void setJoystickButtonMapping(unsigned int button, GameAction action);
    void setDeadzone(float deadzone);

private:
    void setDefaultMappings();
    void updateJoystickInput();
    void applyInputToPlayers();

    std::map<sf::Keyboard::Key, GameAction> m_keyMappings;
    std::map<unsigned int, GameAction> m_joystickButtonMappings;
    std::map<GameAction, bool> m_currentActionStates;
    std::map<GameAction, bool> m_previousActionStates;

    unsigned int m_joystickId = 0;
    float m_deadzone = 20.0f;
    float m_playerSpeed = 400.0f;
};

InputSystem::InputSystem(ECS& ecs) : ISystem(ecs)
{
    for (int i = static_cast<int>(GameAction::MoveUp); i <= static_cast<int>(GameAction::Quit); ++i)
    {
        GameAction action = static_cast<GameAction>(i);
        m_currentActionStates[action] = false;
        m_previousActionStates[action] = false;
    }

    setDefaultMappings();
}

void InputSystem::setDefaultMappings()
{
    m_keyMappings[sf::Keyboard::Up] = GameAction::MoveUp;
    m_keyMappings[sf::Keyboard::Down] = GameAction::MoveDown;
    m_keyMappings[sf::Keyboard::Left] = GameAction::MoveLeft;
    m_keyMappings[sf::Keyboard::Right] = GameAction::MoveRight;
    m_keyMappings[sf::Keyboard::Space] = GameAction::Shoot;
    m_keyMappings[sf::Keyboard::Escape] = GameAction::Quit;
    m_keyMappings[sf::Keyboard::Z] = GameAction::MoveUp;
    m_keyMappings[sf::Keyboard::S] = GameAction::MoveDown;
    m_keyMappings[sf::Keyboard::Q] = GameAction::MoveLeft;
    m_keyMappings[sf::Keyboard::D] = GameAction::MoveRight;

    m_joystickButtonMappings[0] = GameAction::Shoot;
    m_joystickButtonMappings[1] = GameAction::Shoot;
    m_joystickButtonMappings[6] = GameAction::Quit;
    m_joystickButtonMappings[7] = GameAction::Quit;
}

void InputSystem::setKeyMapping(sf::Keyboard::Key key, GameAction action)
{
    m_keyMappings[key] = action;
}

void InputSystem::setJoystickButtonMapping(unsigned int button, GameAction action)
{
    m_joystickButtonMappings[button] = action;
}

void InputSystem::setDeadzone(float deadzone)
{
    m_deadzone = deadzone;
}

void InputSystem::updateJoystickInput()
{
    if (!sf::Joystick::isConnected(m_joystickId))
        return;

    float xAxis = sf::Joystick::getAxisPosition(m_joystickId, sf::Joystick::X);
    float yAxis = sf::Joystick::getAxisPosition(m_joystickId, sf::Joystick::Y);

    // D-pad (PovX and PovY)
    float povX = sf::Joystick::getAxisPosition(m_joystickId, sf::Joystick::PovX);
    float povY = sf::Joystick::getAxisPosition(m_joystickId, sf::Joystick::PovY);

    if (xAxis < -m_deadzone || povX < -m_deadzone) {
        m_currentActionStates[GameAction::MoveLeft] = true;
    }
    if (xAxis > m_deadzone || povX > m_deadzone) {
        m_currentActionStates[GameAction::MoveRight] = true;
    }
    if (yAxis < -m_deadzone || povY > m_deadzone) {
        m_currentActionStates[GameAction::MoveUp] = true;
    }
    if (yAxis > m_deadzone || povY < -m_deadzone) {
        m_currentActionStates[GameAction::MoveDown] = true;
    }

    // Joystick buttons
    for (const auto& [button, action] : m_joystickButtonMappings) {
        if (sf::Joystick::isButtonPressed(m_joystickId, button)) {
            m_currentActionStates[action] = true;
        }
    }
}

void InputSystem::applyInputToPlayers()
{
    auto players = _ecs.getEntitiesByComponents<PlayerController_t, Velocity_t>();

    for (Entity player : players) {
        auto* vel = _ecs.getComponent<Velocity_t>(player);
        auto* ctrl = _ecs.getComponent<PlayerController_t>(player);
        if (!vel || !ctrl) continue;

        vel->x = 0.f;
        vel->y = 0.f;

        if (isActionActive(GameAction::MoveUp))
            vel->y = -m_playerSpeed;
        if (isActionActive(GameAction::MoveDown))
            vel->y = m_playerSpeed;
        if (isActionActive(GameAction::MoveLeft))
            vel->x = -m_playerSpeed;
        if (isActionActive(GameAction::MoveRight))
            vel->x = m_playerSpeed;

        ctrl->isShooting = isActionActive(GameAction::Shoot);
    }
}

void InputSystem::update(double dt)
{
    (void)dt;

    m_previousActionStates = m_currentActionStates;

    // Reset toutes les actions (elles seront remises à true si la touche est pressée)
    for (auto& [action, state] : m_currentActionStates) {
        state = false;
    }

    // Keyboard polling (real-time)
    for (const auto& [key, action] : m_keyMappings) {
        if (sf::Keyboard::isKeyPressed(key)) {
            m_currentActionStates[action] = true;
        }
    }

    sf::Joystick::update();
    updateJoystickInput();
    applyInputToPlayers();
}

bool InputSystem::isActionActive(GameAction action) const
{
    if (m_currentActionStates.count(action)) {
        return m_currentActionStates.at(action);
    }
    return false;
}

bool InputSystem::wasActionPressed(GameAction action) const
{
    if (m_currentActionStates.count(action) && m_previousActionStates.count(action)) {
        return m_currentActionStates.at(action) && !m_previousActionStates.at(action);
    }
    return false;
}

bool InputSystem::wasActionReleased(GameAction action) const
{
    if (m_currentActionStates.count(action) && m_previousActionStates.count(action)) {
        return !m_currentActionStates.at(action) && m_previousActionStates.at(action);
    }
    return false;
}