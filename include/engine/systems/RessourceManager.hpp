/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Ressource Manager
*/

#pragma once
    #include <SFML/Graphics.hpp>
    #include <unordered_map>
    #include <string>
    #include <iostream>
    #include <sstream>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    bool loadTexture(const std::string& id, const std::string& filepath);
    sf::Texture& getTexture(const std::string& id);
private:
    ResourceManager() = default;
    std::unordered_map<std::string, sf::Texture> _textures;
};

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadTexture(const std::string& id, const std::string& filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        std::cerr << "[Resource] Erreur load " << filepath << std::endl;
        return false;
    }
    _textures[id] = std::move(texture);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& id)
{
    auto it = _textures.find(id);
    if (it != _textures.end()) return it->second;

    // Texture par défaut rouge si rien n’est chargé
    static sf::Texture fallback;
    static bool init = false;
    if (!init) {
        sf::Image img;
        img.create(64, 64, sf::Color::Red);
        fallback.loadFromImage(img);
        init = true;
    }
    return fallback;
}