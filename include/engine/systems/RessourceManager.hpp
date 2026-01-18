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
    #include <fstream>
    #include "texturePacker.hpp"

class RessourceManager {
    public:
        RessourceManager() = default;
        ~RessourceManager() = default;

        static RessourceManager& getInstance();

        void initialize(); // Load all assets at start
        void loadTextureFromAtlas(const std::string &atlas);
        sf::Texture& getTexture(std::size_t id);
        sf::Texture& getTexture(const std::string &name);
        sf::IntRect getSpriteRect(const std::string &name);

        TexturePacker packer;
    private:
        std::string getCanonicalName(const std::string &name) const;

        std::unordered_map<std::size_t, sf::Texture> _textures;
        std::unordered_map<std::string, std::size_t> _nameToId;
        std::unordered_map<std::string, std::string> _canonicalNames;  // Maps aliases to canonical names
        std::unordered_map<std::string, sf::IntRect> _spriteRects;
        std::unordered_map<std::string, sf::Image> _atlasImages;
        std::unordered_map<std::string, std::string> _atlasJsons;
};

inline RessourceManager& RessourceManager::getInstance()
{
    static RessourceManager instance;
    return instance;
}

inline void RessourceManager::initialize()
{
    packer.reset();
    packer.loadImageFromPath("player", "assets/sprites/entities/ship1.png");
    packer.loadImageFromPath("enemy", "assets/sprites/entities/ship2.png");
    packer.loadImageFromPath("ship3", "assets/sprites/entities/ship3.png");
    packer.loadImageFromPath("ship4", "assets/sprites/entities/ship4.png");
    packer.loadImageFromPath("bullet", "assets/sprites/entities/bullet.png");
    packer.loadImageFromPath("block", "assets/sprites/entities/block.png");
    packer.loadImageFromPath("caisse", "assets/sprites/entities/caisse.png");

    packer.packTextures();
    packer.generateAtlas("assets/sprites/atlas.png");
    packer.saveJSON("assets/sprites/atlas.json");

    _atlasJsons["entity"] = "assets/sprites/atlas.json";
    if (!_atlasImages["entity"].loadFromFile("assets/sprites/atlas.png")) {
        std::cerr << "Failed to load atlas image: assets/sprites/atlas.png" << std::endl;
        return;
    }
    loadTextureFromAtlas("entity");

    // Add aliases to ensure compatibility with all systems
    if (_nameToId.count("player")) {
        _nameToId["ship"] = _nameToId["player"];
        _nameToId["ship.png"] = _nameToId["player"];
        _canonicalNames["ship"] = "player";
        _canonicalNames["ship.png"] = "player";
    }
    if (_nameToId.count("enemy")) {
        _nameToId["enemy.png"] = _nameToId["enemy"];
        _canonicalNames["enemy.png"] = "enemy";
    }
}

inline void RessourceManager::loadTextureFromAtlas(const std::string &atlas)
{
    if (_atlasImages.find(atlas) == _atlasImages.end()) {
        std::cerr << "Atlas image not found for key: " << atlas << std::endl;
        return;
    }
    sf::Image &image = _atlasImages[atlas];

    if (_atlasJsons.find(atlas) == _atlasJsons.end()) {
        std::cerr << "Atlas JSON not found for key: " << atlas << std::endl;
        return;
    }

    std::ifstream file(_atlasJsons[atlas]);
    if (!file.is_open()) {
        std::cerr << "Failed to load atlas JSON: " << _atlasJsons[atlas] << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("\"id\":") == std::string::npos)
            continue;

        try {
            size_t nameStart = line.find("\"") + 1;
            size_t nameEnd = line.find("\"", nameStart);
            std::string name = line.substr(nameStart, nameEnd - nameStart);

            auto extractValue = [&](const std::string &key) {
                size_t pos = line.find("\"" + key + "\":");
                if (pos == std::string::npos) return -1;
                pos = line.find(":", pos) + 1;
                size_t end = line.find_first_of(",}", pos);
                return std::stoi(line.substr(pos, end - pos));
            };

            int id = extractValue("id");
            int x = extractValue("x");
            int y = extractValue("y");
            int width = extractValue("width");
            int height = extractValue("height");

            std::cout << "Extracted - ID: " << id << ", X: " << x << ", Y: " << y 
                      << ", Width: " << width << ", Height: " << height << ", Name: " << name << std::endl;

            if (id != -1) {
                sf::Texture texture;
                if (texture.loadFromImage(image, sf::IntRect(x, y, width, height))) {
                    _textures[id] = texture;
                    _nameToId[name] = id;
                    _spriteRects[name] = sf::IntRect(0, 0, width, height);
                }
            }
        } catch (...) {
            continue;
        }
    }

    file.close();
}

inline sf::Texture& RessourceManager::getTexture(std::size_t id)
{
    if (_textures.find(id) == _textures.end())
        throw std::runtime_error("Texture ID not found: " + std::to_string(id));

    return _textures[id];
}

inline sf::Texture& RessourceManager::getTexture(const std::string &name)
{
    if (_nameToId.find(name) == _nameToId.end())
        throw std::runtime_error("Texture name not found: " + name);

    return _textures[_nameToId[name]];
}

inline sf::IntRect RessourceManager::getSpriteRect(const std::string &name)
{
    // Check if the exact name exists
    if (_spriteRects.find(name) != _spriteRects.end()) {
        return _spriteRects[name];
    }

    // Check if it's an alias and get the canonical name
    if (_canonicalNames.find(name) != _canonicalNames.end()) {
        std::string canonical = _canonicalNames[name];
        if (_spriteRects.find(canonical) != _spriteRects.end()) {
            return _spriteRects[canonical];
        }
    }

    // Not found, return default
    std::cerr << "Sprite rect not found for: " << name << ", returning default 64x64" << std::endl;
    return sf::IntRect(0, 0, 64, 64);
}
