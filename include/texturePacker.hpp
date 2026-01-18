/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** texturePacker
*/

#ifndef TEXTUREPACKER_HPP_
    #define TEXTUREPACKER_HPP_

    #include <SFML/Graphics.hpp>
    #include <SFML/Graphics/Image.hpp>
    #include <string>
    #include <vector>
    #include <fstream>

static constexpr size_t MAX_ATLAS_WIDTH = 1024; // Increased to accommodate larger assets

typedef struct imageItem_s {
    std::string name;
    sf::Image image;
    int width;
    int height;
} imageItem_t;

typedef struct placedItem_s {
    imageItem_t item;
    sf::IntRect rect;
    int x;
    int y;
} placedItem_t;

typedef struct shelf_s {
    int x;
    int y;
    int height;
    int remainingWidth;     // Max width - x
} shelf_t;

class TexturePacker {
    public:
        TexturePacker();
        ~TexturePacker();

        void loadImageFromPath(const std::string &name, const std::string &imagePath);
        void packTextures();
        void generateAtlas(const std::string &outputPath);
        void saveJSON(const std::string &outputPath);

        void reset();
    private:
        std::size_t nbSprites = 0;

        std::vector<imageItem_t> _images;
        std::vector<shelf_t> _shelves;
        std::vector<placedItem_t> _placed;
};

TexturePacker::TexturePacker() {}

TexturePacker::~TexturePacker() {
    reset();
}

void TexturePacker::loadImageFromPath(const std::string &name, const std::string &imagePath) {
    imageItem_t newItem;
    newItem.name = name;
    if (!newItem.image.loadFromFile(imagePath)) {
        std::cerr << "TexturePacker: Failed to load image " << imagePath << std::endl;
        return;
    }
    newItem.width = newItem.image.getSize().x;
    newItem.height = newItem.image.getSize().y;
    _images.push_back(newItem);
}

void TexturePacker::packTextures() {
    if (_images.empty())
        return;

    std::sort(_images.begin(), _images.end(),
        [](const imageItem_t &a, const imageItem_t &b) {
            return (a.height > b.height);
        });

    _placed.clear();
    _shelves.clear();

    for (const auto &img : _images) {
        bool placed = false;
        int placedX = 0;
        int placedY = 0;

        for (auto &shelf : _shelves) {
            if (img.width <= shelf.remainingWidth && img.height <= shelf.height) {
                placedX = shelf.x;
                placedY = shelf.y;
                shelf.x += img.width;
                shelf.remainingWidth -= img.width;
                placed = true;
                break;
            }
        }
        if (!placed) {
            if (img.width > MAX_ATLAS_WIDTH)
                continue; // Image too wide to fit in atlas

            shelf_t newShelf;
            newShelf.x = img.width;
            newShelf.y = _shelves.empty() ? 0 : _shelves.back().y + _shelves.back().height;
            newShelf.height = img.height;
            newShelf.remainingWidth = MAX_ATLAS_WIDTH - img.width;
            placedX = 0;
            placedY = newShelf.y;
            _shelves.push_back(newShelf);
        }
        placedItem_t placedItem;
        placedItem.item = img;
        placedItem.rect = sf::IntRect(placedX, placedY, img.width, img.height);
        placedItem.x = placedX;
        placedItem.y = placedY;
        _placed.push_back(placedItem);
    }
}

void TexturePacker::generateAtlas(const std::string &outputPath) {
    if (_placed.empty())
        return;

    int atlasHeight = 0;
    for (const auto &shelf : _shelves) {
        atlasHeight += shelf.height;
    }

    sf::Image atlasImage;
    atlasImage.create(MAX_ATLAS_WIDTH, atlasHeight, sf::Color::Transparent);

    for (const auto &p : _placed) {
        atlasImage.copy(p.item.image, p.x, p.y);
    }

    atlasImage.saveToFile(outputPath);
}

void TexturePacker::saveJSON(const std::string &outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) return;

    file << "{\n";
    file << "  \"sprites\": {\n";

    for (size_t i = 0; i < _placed.size(); ++i) {
        const auto& p = _placed[i];
        file << "   \"" << p.item.name << "\": {";
        file << "\"id\": " << nbSprites << ", ";
        file << "\"x\": " << p.rect.left << ", ";
        file << "\"y\": " << p.rect.top << ", ";
        file << "\"width\": " << p.rect.width << ", ";
        file << "\"height\": " << p.rect.height << "";
        file << "}";
        if (i < _placed.size() - 1) {
            file << ",";
        }
        file << "\n";
        nbSprites ++;
    }

    file << "  }\n";
    file << "}\n";
    file.close();
}

void TexturePacker::reset() {
    _images.clear();
    _shelves.clear();
    _placed.clear();
    nbSprites = 0;
}

#endif /* !TEXTUREPACKER_HPP_ */
