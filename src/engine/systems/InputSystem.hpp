/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Input System
*/

#pragma once

#include <map>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Quit
};

class InputSystem {
public:
    InputSystem();
    ~InputSystem() = default;

    void update(sf::Window& window);
    bool isActionActive(GameAction action) const;
    bool wasActionPressed(GameAction action) const;
    bool wasActionReleased(GameAction action) const;
    void setKeyMapping(sf::Keyboard::Key key, GameAction action);
    void setJoystickButtonMapping(unsigned int button, GameAction action);
    void setDeadzone(float deadzone);

private:
    void setDefaultMappings();
    void updateJoystickInput();
    
    std::map<sf::Keyboard::Key, GameAction> m_keyMappings;
    std::map<unsigned int, GameAction> m_joystickButtonMappings;
    std::map<GameAction, bool> m_currentActionStates;
    std::map<GameAction, bool> m_previousActionStates;
    
    unsigned int m_joystickId = 0;
    float m_deadzone = 20.0f;
};
