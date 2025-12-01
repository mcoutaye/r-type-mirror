/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Input System
*/

#include "InputSystem.hpp"

InputSystem::InputSystem() {
    for (int i = static_cast<int>(GameAction::MoveUp); i <= static_cast<int>(GameAction::Quit); ++i) {
        GameAction action = static_cast<GameAction>(i);
        m_currentActionStates[action] = false;
        m_previousActionStates[action] = false;
    }
    
    setDefaultMappings();
}

void InputSystem::setDefaultMappings() {
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
}

void InputSystem::setKeyMapping(sf::Keyboard::Key key, GameAction action) {
    m_keyMappings[key] = action;
}

void InputSystem::update(sf::Window& window) {
    m_previousActionStates = m_currentActionStates;

    sf::Event event;
    while (window.pollEvent(event)) {
        
        if (event.type == sf::Event::Closed) {
            m_currentActionStates[GameAction::Quit] = true;
        }

        if (event.type == sf::Event::KeyPressed) {
            if (m_keyMappings.count(event.key.code)) {
                GameAction action = m_keyMappings.at(event.key.code);
                m_currentActionStates[action] = true;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            if (m_keyMappings.count(event.key.code)) {
                GameAction action = m_keyMappings.at(event.key.code);
                m_currentActionStates[action] = false;
            }
        }
    }
}

bool InputSystem::isActionActive(GameAction action) const {
    if (m_currentActionStates.count(action)) {
        return m_currentActionStates.at(action);
    }
    return false;
}

bool InputSystem::wasActionPressed(GameAction action) const {
    if (m_currentActionStates.count(action) && m_previousActionStates.count(action)) {
        return m_currentActionStates.at(action) && !m_previousActionStates.at(action);
    }
    return false;
}

bool InputSystem::wasActionReleased(GameAction action) const {
    if (m_currentActionStates.count(action) && m_previousActionStates.count(action)) {
        return !m_currentActionStates.at(action) && m_previousActionStates.at(action);
    }
    return false;
}