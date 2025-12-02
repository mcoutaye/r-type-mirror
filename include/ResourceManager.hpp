// ============================================
// 2. RESOURCE MANAGER (singleton simple)
// ============================================

// ResourceManager.hpp
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
