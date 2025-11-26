// ResourceManager.cpp
#include "ResourceManager.hpp"
#include <iostream>

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

sf::Texture& ResourceManager::getTexture(const std::string& id) {
    static sf::Texture dummy;
    auto it = _textures.find(id);
    return (it != _textures.end()) ? it->second : dummy;
}