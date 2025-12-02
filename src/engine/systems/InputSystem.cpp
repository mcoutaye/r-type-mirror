/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Input System
*/

#include "InputSystem.hpp"

InputSystem::InputSystem() {
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

void InputSystem::update(sf::Window& window)
{
    m_previousActionStates = m_currentActionStates;

    for (auto& [action, state] : m_currentActionStates) {
        if (action == GameAction::MoveUp || action == GameAction::MoveDown ||
            action == GameAction::MoveLeft || action == GameAction::MoveRight) {
            state = false;
        }
    }

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
        if (event.type == sf::Event::JoystickButtonPressed) {
            if (m_joystickButtonMappings.count(event.joystickButton.button)) {
                GameAction action = m_joystickButtonMappings.at(event.joystickButton.button);
                m_currentActionStates[action] = true;
            }
        }

        if (event.type == sf::Event::JoystickButtonReleased) {
            if (m_joystickButtonMappings.count(event.joystickButton.button)) {
                GameAction action = m_joystickButtonMappings.at(event.joystickButton.button);
                m_currentActionStates[action] = false;
            }
        }
    }
    sf::Joystick::update();
    updateJoystickInput();
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