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

class ResourceManager {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        static ResourceManager& getInstance();

        bool initialize(); // Load all assets at start
        void loadTextureFromAtlas(const std::string &atlas);
        sf::Texture& getTexture(std::size_t id);
        sf::Texture& getTexture(const std::string &name);
        sf::IntRect getSpriteRect(const std::string &name);

        // === FONTS ===
        bool loadFont(const std::string& id, const std::string& filepath);
        const sf::Font& getFont(const std::string& id) const;

    private:
        TexturePacker packer;

        std::unordered_map<std::size_t, sf::Texture> _textures;
        std::unordered_map<std::string, std::size_t> _nameToId;
        std::unordered_map<std::string, std::string> _canonicalNames;  // Maps aliases to canonical names
        std::unordered_map<std::string, sf::IntRect> _spriteRects;
        std::unordered_map<std::string, sf::Image> _atlasImages;
        std::unordered_map<std::string, std::string> _atlasJsons;

        std::unordered_map<std::string, sf::Font> _fonts;
};

inline ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return instance;
}

inline bool ResourceManager::initialize()
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
        return false;
    }
    loadTextureFromAtlas("entity");
    return true;
}

// === TEXTURES ===
// bool ResourceManager::loadTexture(const std::string& id, const std::string& filepath)
// {
//     sf::Texture texture;
//     if (!texture.loadFromFile(filepath)) {
//         std::cerr << "[Resource] Erreur chargement texture : " << filepath << std::endl;
//         return false;
//     }
//     auto it = _textures.find(id);
//     if (it != _textures.end()) {
//         return it->second;
//     }

//     // Fallback : texture rouge
//     static sf::Texture fallback;
//     static bool initialized = false;
//     if (!initialized) {
//         sf::Image img;
//         img.create(64, 64, sf::Color::Red);
//         fallback.loadFromImage(img);
//         initialized = true;
//     }
//     std::cerr << "[Resource] Texture manquante : " << id << " → fallback rouge" << std::endl;
//     return fallback;
// }


inline void ResourceManager::loadTextureFromAtlas(const std::string &atlas)
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

inline sf::Texture& ResourceManager::getTexture(std::size_t id)
{
    auto it = _textures.find(id);
    if (it != _textures.end()) {
        return it->second;
    }

    // Fallback: red texture
    static sf::Texture fallback;
    static bool initialized = false;
    if (!initialized) {
        sf::Image img;
        img.create(64, 64, sf::Color::Red);
        fallback.loadFromImage(img);
        initialized = true;
    }
    std::cerr << "[Resource] Texture ID not found: " << id << " → fallback red texture" << std::endl;
    return fallback;
}

inline sf::Texture& ResourceManager::getTexture(const std::string &name)
{
    auto it = _nameToId.find(name);
    if (it != _nameToId.end()) {
        return _textures[it->second];
    }

    // Fallback: red texture
    static sf::Texture fallback;
    static bool initialized = false;
    if (!initialized) {
        sf::Image img;
        img.create(64, 64, sf::Color::Red);
        fallback.loadFromImage(img);
        initialized = true;
    }
    std::cerr << "[Resource] Texture name not found: " << name << " → fallback red texture" << std::endl;
    return fallback;
}

inline sf::IntRect ResourceManager::getSpriteRect(const std::string &name)
{
    // Check if the exact name exists
    if (_spriteRects.find(name) != _spriteRects.end()) {
        return _spriteRects[name];
    }

    // Not found, return default
    std::cerr << "Sprite rect not found for: " << name << ", returning default 64x64" << std::endl;
    return sf::IntRect(0, 0, 64, 64);
}

// === FONTS ===
inline bool ResourceManager::loadFont(const std::string& id, const std::string& filepath)
{
    sf::Font font;
    if (!font.loadFromFile(filepath)) {
        std::cerr << "[Resource] Erreur chargement police : " << filepath << std::endl;
        return false;
    }
    _fonts[id] = std::move(font);
    return true;
}

inline const sf::Font& ResourceManager::getFont(const std::string& id) const
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
        if (!fallback.loadFromFile("assets/font/arial.ttf") &&
            !fallback.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
            !fallback.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "[Resource] Aucune police fallback disponible !" << std::endl;
        }
        initialized = true;
    }
    std::cerr << "[Resource] Police manquante : " << id << " → fallback" << std::endl;
    return fallback;
}
