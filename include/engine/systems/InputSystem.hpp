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
#include <set>

// GameAction est maintenant défini dans Components.hpp

class InputSystem : public ISystem {
public:
    InputSystem(ECS& ecs);
    ~InputSystem() = default;

    void update(double dt) override;

    bool isActionActive(GameAction action) const;
    bool wasActionPressed(GameAction action) const;
    bool wasActionReleased(GameAction action) const;
    
    // Key remapping methods
    void setKeyMapping(sf::Keyboard::Key key, GameAction action);
    void removeKeyMapping(sf::Keyboard::Key key);
    void clearKeyMappings();
    sf::Keyboard::Key getKeyForAction(GameAction action) const;
    
    // Joystick remapping methods
    void setJoystickButtonMapping(unsigned int button, GameAction action);
    void removeJoystickButtonMapping(unsigned int button);
    void clearJoystickButtonMappings();
    unsigned int getJoystickButtonForAction(GameAction action) const;
    
    // Settings
    void setDeadzone(float deadzone);
    void resetToDefaultMappings();

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
    for (int i = static_cast<int>(GameAction::MoveUp); i <= static_cast<int>(GameAction::ToggleDebug); ++i)
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
    m_keyMappings[sf::Keyboard::Escape] = GameAction::Pause;  // Escape = Pause en jeu
    m_keyMappings[sf::Keyboard::Z] = GameAction::MoveUp;
    m_keyMappings[sf::Keyboard::S] = GameAction::MoveDown;
    m_keyMappings[sf::Keyboard::Q] = GameAction::MoveLeft;
    m_keyMappings[sf::Keyboard::D] = GameAction::MoveRight;
    m_keyMappings[sf::Keyboard::F1] = GameAction::ToggleDebug;
    m_keyMappings[sf::Keyboard::Enter] = GameAction::MenuSelect;
    m_keyMappings[sf::Keyboard::P] = GameAction::Pause;  // P aussi pour Pause

    m_joystickButtonMappings[0] = GameAction::Shoot;
    m_joystickButtonMappings[1] = GameAction::Shoot;
    m_joystickButtonMappings[6] = GameAction::Pause;  // Start/Options = Pause
    m_joystickButtonMappings[7] = GameAction::Pause;
}

void InputSystem::setKeyMapping(sf::Keyboard::Key key, GameAction action)
{
    // Touches protégées qui ne peuvent jamais être supprimées (flèches, Enter)
    static const std::set<sf::Keyboard::Key> protectedKeys = {
        sf::Keyboard::Up,
        sf::Keyboard::Down,
        sf::Keyboard::Left,
        sf::Keyboard::Right,
        sf::Keyboard::Return
    };
    
    // 1. Retire cette touche de toute autre action où elle pourrait être
    //    (pour qu'une touche ne puisse pas faire deux choses)
    for (auto it = m_keyMappings.begin(); it != m_keyMappings.end();) {
        if (it->first == key && it->second != action) {
            it = m_keyMappings.erase(it);
        } else {
            ++it;
        }
    }
    
    // 2. Retire les anciennes touches NON-PROTÉGÉES de cette action
    //    (pour remplacer la touche remappable précédente)
    for (auto it = m_keyMappings.begin(); it != m_keyMappings.end();) {
        if (it->second == action && it->first != key && 
            protectedKeys.find(it->first) == protectedKeys.end()) {
            it = m_keyMappings.erase(it);
        } else {
            ++it;
        }
    }
    
    // 3. Ajoute le nouveau mapping
    m_keyMappings[key] = action;
}

void InputSystem::removeKeyMapping(sf::Keyboard::Key key)
{
    m_keyMappings.erase(key);
}

void InputSystem::clearKeyMappings()
{
    m_keyMappings.clear();
}

sf::Keyboard::Key InputSystem::getKeyForAction(GameAction action) const
{
    for (const auto& [key, act] : m_keyMappings) {
        if (act == action) {
            return key;
        }
    }
    return sf::Keyboard::Unknown;
}

void InputSystem::setJoystickButtonMapping(unsigned int button, GameAction action)
{
    // Remove this button from other actions first
    for (auto it = m_joystickButtonMappings.begin(); it != m_joystickButtonMappings.end();) {
        if (it->second == action && it->first != button) {
            it = m_joystickButtonMappings.erase(it);
        } else {
            ++it;
        }
    }
    m_joystickButtonMappings[button] = action;
}

void InputSystem::removeJoystickButtonMapping(unsigned int button)
{
    m_joystickButtonMappings.erase(button);
}

void InputSystem::clearJoystickButtonMappings()
{
    m_joystickButtonMappings.clear();
}

unsigned int InputSystem::getJoystickButtonForAction(GameAction action) const
{
    for (const auto& [button, act] : m_joystickButtonMappings) {
        if (act == action) {
            return button;
        }
    }
    return static_cast<unsigned int>(-1);
}

void InputSystem::resetToDefaultMappings()
{
    clearKeyMappings();
    clearJoystickButtonMappings();
    setDefaultMappings();
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