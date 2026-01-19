/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** Menu
*/

#pragma once
#include <vector>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include "../systems/Components.hpp"

struct MenuAction {
    std::string id;  // Identifiant de l'action sous forme de chaîne de caractères
    std::function<void()> callback;  // Fonction à appeler quand l'action est déclenchée
};

struct MenuItem {
    std::string text;
    MenuAction action;
    float xPosition;
    float yPosition;
    std::string fontId;
    uint32_t fontSize;
    sf::Color color;
    sf::Color selectedColor;
    float scale;
    bool centered;
    bool visible = true;
    bool isSelectable = true;  // Peut être navigué et sélectionné
};

struct Menu {
    std::string id;
    std::vector<MenuItem> items;
    std::string title;
    float titleXPosition;
    float titleYPosition;
    std::string titleFontId;
    uint32_t titleFontSize;
    sf::Color titleColor;
    bool titleCentered;
    std::string backgroundMusicId;
};