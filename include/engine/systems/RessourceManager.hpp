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

    // === TEXTURES ===
    bool loadTexture(const std::string& id, const std::string& filepath);
    sf::Texture& getTexture(const std::string& id);

    // === FONTS ===
    bool loadFont(const std::string& id, const std::string& filepath);
    const sf::Font& getFont(const std::string& id) const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, sf::Texture> _textures;
    std::unordered_map<std::string, sf::Font> _fonts;
};

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

// === TEXTURES ===
bool ResourceManager::loadTexture(const std::string& id, const std::string& filepath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        std::cerr << "[Resource] Erreur chargement texture : " << filepath << std::endl;
        return false;
    }
    _textures[id] = std::move(texture);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& id)
{
    auto it = _textures.find(id);
    if (it != _textures.end()) {
        return it->second;
    }

    // Fallback : texture rouge
    static sf::Texture fallback;
    static bool initialized = false;
    if (!initialized) {
        sf::Image img;
        img.create(64, 64, sf::Color::Red);
        fallback.loadFromImage(img);
        initialized = true;
    }
    std::cerr << "[Resource] Texture manquante : " << id << " → fallback rouge" << std::endl;
    return fallback;
}

// === FONTS ===
bool ResourceManager::loadFont(const std::string& id, const std::string& filepath)
{
    sf::Font font;
    if (!font.loadFromFile(filepath)) {
        std::cerr << "[Resource] Erreur chargement police : " << filepath << std::endl;
        return false;
    }
    _fonts[id] = std::move(font);
    return true;
}

const sf::Font& ResourceManager::getFont(const std::string& id) const
{
    auto it = _fonts.find(id);
    if (it != _fonts.end()) {
        return it->second;
    }

    // Fallback : police système basique ou intégrée
    static sf::Font fallback;
    static bool initialized = false;
    if (!initialized) {
        // Essaie de charger une police par défaut du système
        if (!fallback.loadFromFile("assets/fonts/arial.ttf") &&
            !fallback.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
            !fallback.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "[Resource] Aucune police fallback disponible !" << std::endl;
        }
        initialized = true;
    }
    std::cerr << "[Resource] Police manquante : " << id << " → fallback" << std::endl;
    return fallback;
}