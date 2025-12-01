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

enum class GameAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Quit,
    Unknown
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

private:
    void setDefaultMappings();
    std::map<sf::Keyboard::Key, GameAction> m_keyMappings;
    std::map<GameAction, bool> m_currentActionStates;
    std::map<GameAction, bool> m_previousActionStates;
};
