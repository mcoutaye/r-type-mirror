/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** MenuSystem
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/SceneManager.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/SoundSystem.hpp"
#include "engine/EntityFactory.hpp"
#include "engine/StageFactory.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

class MenuSystem : public ISystem {
public:
    explicit MenuSystem(ECS& ecs, InputSystem& inputSystem);
    void update(double dt) override;
    void setSceneManager(std::unique_ptr<SceneManager> sceneManager);
    void registerAction(const std::string& actionId, std::function<void()> callback);
    void setEnabled(bool enabled);
    void setSoundSystem(SoundSystem* soundSystem);
    void setWindow(sf::RenderWindow* window);
    void handleMouseInput(const sf::Event& event);
    bool isWaitingForKeyInput() const { return _waitingForKeyEntity != static_cast<Entity>(-1); }

private:
    void handleInput();
    void handleSliders();
    void handleKeybindButtons();
    void updateHighlight();
    void executeSelectedAction();
    void updateDynamicTexts();
    std::string getKeyName(sf::Keyboard::Key key);
    
    std::vector<Entity> _menuItems;
    bool _actionTriggered = false;
    int _currentSelection = 0;
    std::unique_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, std::function<void()>> _actionCallbacks;
    bool _enabled = true;
    InputSystem& _inputSystem;
    SoundSystem* _soundSystem = nullptr;
    sf::RenderWindow* _window = nullptr;
    float _inputCooldown = 0.0f;
    const float _inputDelay = 0.15f;
    Entity _waitingForKeyEntity = static_cast<Entity>(-1);
    bool _waitingForKeyRelease = false;  // Attend que la touche d'activation soit relâchée
};

MenuSystem::MenuSystem(ECS& ecs, InputSystem& inputSystem) : ISystem(ecs), _inputSystem(inputSystem) {}

void MenuSystem::setSceneManager(std::unique_ptr<SceneManager> sceneManager)
{
    _sceneManager = std::move(sceneManager);
}

void MenuSystem::registerAction(const std::string& actionId, std::function<void()> callback)
{
    _actionCallbacks[actionId] = callback;
}

void MenuSystem::setEnabled(bool enabled)
{
    _enabled = enabled;
    _actionTriggered = false;
}

void MenuSystem::update(double dt)
{
    if (!_enabled) return;
    
    // Décrémente le cooldown
    if (_inputCooldown > 0.0f) {
        _inputCooldown -= static_cast<float>(dt);
    }
    
    _menuItems = _ecs.getEntitiesByComponents<Position_t, Text_t, MenuItem_t>();
    
    // Filtre pour ne garder que les éléments visibles
    _menuItems.erase(
        std::remove_if(_menuItems.begin(), _menuItems.end(), [this](Entity e) {
            auto* text = _ecs.getComponent<Text_t>(e);
            return !text || !text->visible;
        }),
        _menuItems.end()
    );
    
    // Trie les items par position Y (du haut vers le bas)
    std::sort(_menuItems.begin(), _menuItems.end(), [this](Entity a, Entity b) {
        auto* posA = _ecs.getComponent<Position_t>(a);
        auto* posB = _ecs.getComponent<Position_t>(b);
        if (!posA || !posB) return false;
        return posA->y < posB->y;
    });
    
    if (_menuItems.empty()) return;

    // Vérifie que _currentSelection est valide
    if (_currentSelection < 0 || _currentSelection >= static_cast<int>(_menuItems.size())) {
        _currentSelection = 0;
        // Trouve le premier élément sélectionnable
        for (size_t i = 0; i < _menuItems.size(); ++i) {
            auto* item = _ecs.getComponent<MenuItem_t>(_menuItems[i]);
            if (item && item->isSelectable) {
                _currentSelection = static_cast<int>(i);
                break;
            }
        }
    }

    // NE PAS réinitialiser _actionTriggered ici car il peut avoir été mis à true par handleMouseInput()
    
    handleInput();
    handleSliders();
    handleKeybindButtons();
    updateDynamicTexts();
    updateHighlight();
    
    if (_actionTriggered) {
        executeSelectedAction();
        _actionTriggered = false;  // Reset après exécution
    }
}

void MenuSystem::handleInput()
{
    // Ne gère les inputs que si MenuSystem est activé
    if (!_enabled) return;

    // Si on attend une touche pour un keybind, ne traite AUCUN input normal
    if (_waitingForKeyEntity != static_cast<Entity>(-1)) return;

    // Ne traite les inputs que si le cooldown est écoulé
    if (_inputCooldown > 0.0f) return;

    // Navigation dans le menu - Les flèches sont TOUJOURS actives (pas remappables)
    // On vérifie aussi les touches remappées pour le jeu (Z/S) comme alternative
    bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || 
                       _inputSystem.isActionActive(GameAction::MoveDown);
    bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 
                     _inputSystem.isActionActive(GameAction::MoveUp);
    
    if (downPressed) {
        int nextSelection = (_currentSelection + 1) % _menuItems.size();
        // Trouve le prochain élément visible et sélectionnable
        int startSelection = nextSelection;
        while (nextSelection != _currentSelection) {
            Entity nextEntity = _menuItems[nextSelection];
            auto* text = _ecs.getComponent<Text_t>(nextEntity);
            auto* item = _ecs.getComponent<MenuItem_t>(nextEntity);
            if (text && text->visible && item && item->isSelectable) {
                _currentSelection = nextSelection;
                _inputCooldown = _inputDelay;
                break;
            }
            nextSelection = (nextSelection + 1) % _menuItems.size();
            if (nextSelection == startSelection) break; // Évite boucle infinie
        }
    }
    else if (upPressed) {
        int prevSelection = (_currentSelection - 1 + _menuItems.size()) % _menuItems.size();
        // Trouve l'élément visible et sélectionnable précédent
        int startSelection = prevSelection;
        while (prevSelection != _currentSelection) {
            Entity prevEntity = _menuItems[prevSelection];
            auto* text = _ecs.getComponent<Text_t>(prevEntity);
            auto* item = _ecs.getComponent<MenuItem_t>(prevEntity);
            if (text && text->visible && item && item->isSelectable) {
                _currentSelection = prevSelection;
                _inputCooldown = _inputDelay;
                break;
            }
            prevSelection = (prevSelection - 1 + _menuItems.size()) % _menuItems.size();
            if (prevSelection == startSelection) break; // Évite boucle infinie
        }
    }

    // Détection de la touche pour déclencher une action
    // Entrée est TOUJOURS active, Space utilise l'action remappable
    bool selectPressed = _inputSystem.isActionActive(GameAction::Shoot) || 
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Return);
    
    if (selectPressed && !_actionTriggered) {
        Entity selected = _menuItems[_currentSelection];
        auto* text = _ecs.getComponent<Text_t>(selected);
        auto* item = _ecs.getComponent<MenuItem_t>(selected);
        if (text && text->visible && item && item->isSelectable) {
            _actionTriggered = true;
            _inputCooldown = _inputDelay;
        }
    }
}

void MenuSystem::updateHighlight()
{
    // Réinitialise tous les éléments comme non sélectionnés
    for (Entity e : _menuItems) {
        auto* item = _ecs.getComponent<MenuItem_t>(e);
        auto* text = _ecs.getComponent<Text_t>(e);
        if (!item || !text) continue;

        item->isSelected = false;  // Désélectionne tous les éléments
        text->color = text->originalColor;  // Utilise la couleur originale
        if (auto* drawable = _ecs.getComponent<Drawable_t>(e)) {
            drawable->scale = 1.0f;  // Réinitialise l'échelle normale
        }
    }

    // Vérifie que _currentSelection est valide
    if (_currentSelection >= 0 && _currentSelection < static_cast<int>(_menuItems.size())) {
        Entity selectedEntity = _menuItems[_currentSelection];
        auto* selectedItem = _ecs.getComponent<MenuItem_t>(selectedEntity);
        auto* selectedText = _ecs.getComponent<Text_t>(selectedEntity);
        auto* selectedHighlight = _ecs.getComponent<Highlight_t>(selectedEntity);

        if (selectedItem && selectedText && selectedHighlight && selectedText->visible) {
            selectedItem->isSelected = true;  // Sélectionne uniquement l'élément courant
            selectedText->color = selectedHighlight->selectedColor;  // Applique la couleur de surbrillance
            if (auto* drawable = _ecs.getComponent<Drawable_t>(selectedEntity)) {
                drawable->scale = selectedHighlight->selectedScale;  // Applique l'échelle de surbrillance
            }
        }
    }
}

void MenuSystem::executeSelectedAction()
{
    // Vérifie que la sélection est valide
    if (_currentSelection < 0 || _currentSelection >= static_cast<int>(_menuItems.size())) {
        return;
    }

    // Récupère l'élément sélectionné
    Entity selected = _menuItems[_currentSelection];
    
    // Si c'est un keybind button, ne pas exécuter d'action (c'est géré par handleKeybindButtons)
    if (_ecs.hasComponent<KeybindButton_t>(selected)) {
        return;
    }
    
    auto* item = _ecs.getComponent<MenuItem_t>(selected);
    auto* text = _ecs.getComponent<Text_t>(selected);

    // Vérifie que l'élément est visible
    if (!item || !text || !text->visible || item->action.id.empty()) {
        return;  // Ne fait rien si l'élément n'est pas visible ou n'a pas d'action
    }

    // Exécute le callback associé à l'action
    if (_actionCallbacks.find(item->action.id) != _actionCallbacks.end()) {
        _actionCallbacks[item->action.id]();
    }

    // Joue un son de sélection
    Entity clickSound = _ecs.createEntity();
    _ecs.addComponent(clickSound, PlaySound_t{"click.ogg", 80.f});
}

void MenuSystem::setSoundSystem(SoundSystem* soundSystem)
{
    _soundSystem = soundSystem;
}

void MenuSystem::setWindow(sf::RenderWindow* window)
{
    _window = window;
}

void MenuSystem::handleMouseInput(const sf::Event& event)
{
    if (!_enabled || !_window) return;
    
    // Gestion de l'entrée de touche pour les keybinds (PRIORITAIRE)
    if (_waitingForKeyEntity != static_cast<Entity>(-1)) {
        auto* keybind = _ecs.getComponent<KeybindButton_t>(_waitingForKeyEntity);
        if (!keybind) {
            _waitingForKeyEntity = static_cast<Entity>(-1);
            _waitingForKeyRelease = false;
            return;
        }
        
        // Attend que les touches d'activation soient relâchées d'abord
        if (_waitingForKeyRelease) {
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Return) {
                    _waitingForKeyRelease = false;  // OK, on peut capturer maintenant
                }
            }
            return;  // Ne capture rien tant que pas relâché
        }
        
        // Maintenant on peut capturer une nouvelle touche
        if (event.type == sf::Event::KeyPressed) {
            // Escape pour annuler
            if (event.key.code == sf::Keyboard::Escape) {
                keybind->isWaitingForInput = false;
                _waitingForKeyEntity = static_cast<Entity>(-1);
                _waitingForKeyRelease = false;
                _inputCooldown = _inputDelay;
                return;
            }
            
            // Sauvegarde la nouvelle touche
            keybind->currentKey = event.key.code;
            keybind->isWaitingForInput = false;
            
            // Met à jour le mapping
            _inputSystem.setKeyMapping(event.key.code, keybind->action);
            
            _waitingForKeyEntity = static_cast<Entity>(-1);
            _waitingForKeyRelease = false;
            _inputCooldown = _inputDelay * 2.0f;
        }
        return;
    }
    
    // Gestion des clics souris (seulement si on n'attend pas de touche)
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
        
        for (size_t i = 0; i < _menuItems.size(); ++i) {
            Entity entity = _menuItems[i];
            auto* pos = _ecs.getComponent<Position_t>(entity);
            auto* text = _ecs.getComponent<Text_t>(entity);
            auto* item = _ecs.getComponent<MenuItem_t>(entity);
            
            if (!pos || !text || !item || !text->visible || !item->isSelectable) continue;
            
            // Zone cliquable approximative autour du texte
            float width = 400.f;
            float height = 50.f;
            
            if (mousePos.x >= pos->x - width/2 && mousePos.x <= pos->x + width/2 &&
                mousePos.y >= pos->y - height/2 && mousePos.y <= pos->y + height/2) {
                _currentSelection = static_cast<int>(i);
                
                // Si c'est un keybind button, active l'attente de touche
                auto* keybind = _ecs.getComponent<KeybindButton_t>(entity);
                if (keybind) {
                    keybind->isWaitingForInput = true;
                    _waitingForKeyEntity = entity;
                    _waitingForKeyRelease = false;  // Clic souris = pas besoin d'attendre
                } else {
                    // Sinon exécute l'action
                    _actionTriggered = true;
                }
                return;
            }
        }
    }
}

void MenuSystem::handleSliders()
{
    if (!_enabled || _inputCooldown > 0.0f) return;
    
    if (_currentSelection < 0 || _currentSelection >= static_cast<int>(_menuItems.size())) return;
    
    Entity selectedEntity = _menuItems[_currentSelection];
    auto* slider = _ecs.getComponent<Slider_t>(selectedEntity);
    
    if (!slider) return;
    
    bool changed = false;
    
    // Les flèches gauche/droite sont TOUJOURS actives pour les sliders
    // On vérifie aussi les touches remappées comme alternative
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || 
        _inputSystem.isActionActive(GameAction::MoveLeft)) {
        slider->currentValue = std::max(slider->minValue, slider->currentValue - slider->step);
        changed = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || 
               _inputSystem.isActionActive(GameAction::MoveRight)) {
        slider->currentValue = std::min(slider->maxValue, slider->currentValue + slider->step);
        changed = true;
    }
    
    if (changed) {
        _inputCooldown = _inputDelay;
        
        // Met à jour le volume
        if (_soundSystem) {
            if (slider->linkedSetting == "music_volume") {
                _soundSystem->setMusicVolume(slider->currentValue);
            } else if (slider->linkedSetting == "sfx_volume") {
                _soundSystem->setSoundVolume(slider->currentValue);
            }
        }
    }
}

void MenuSystem::handleKeybindButtons()
{
    // Cette fonction gère uniquement l'activation du mode keybind par clavier (Enter/Space)
    // La capture de touche elle-même est gérée par handleMouseInput via les événements
    
    // Si on attend déjà une touche, ne rien faire (géré par handleMouseInput)
    if (_waitingForKeyEntity != static_cast<Entity>(-1)) return;
    
    // Si l'élément sélectionné est un keybind et qu'on appuie sur Enter/Space
    if (_currentSelection >= 0 && _currentSelection < static_cast<int>(_menuItems.size())) {
        Entity selectedEntity = _menuItems[_currentSelection];
        auto* keybind = _ecs.getComponent<KeybindButton_t>(selectedEntity);
        
        if (keybind && _actionTriggered) {
            keybind->isWaitingForInput = true;
            _waitingForKeyEntity = selectedEntity;
            _waitingForKeyRelease = true;  // Attend que Space/Enter soit relâché
            _inputCooldown = _inputDelay;
            _actionTriggered = false; // Consomme l'action
        }
    }
}

void MenuSystem::updateDynamicTexts()
{
    auto dynamicTextEntities = _ecs.getEntitiesByComponents<Text_t, DynamicText_t>();
    
    for (Entity entity : dynamicTextEntities) {
        auto* dynText = _ecs.getComponent<DynamicText_t>(entity);
        auto* text = _ecs.getComponent<Text_t>(entity);
        
        if (!dynText || !text) continue;
        
        if (dynText->valueGetter) {
            std::string fullText = dynText->prefix + dynText->valueGetter() + dynText->suffix;
            std::snprintf(text->text, sizeof(text->text), "%s", fullText.c_str());
        }
    }
    
    // Met à jour les textes des keybind buttons
    auto keybindEntities = _ecs.getEntitiesByComponents<Text_t, KeybindButton_t>();
    for (Entity entity : keybindEntities) {
        auto* text = _ecs.getComponent<Text_t>(entity);
        auto* keybind = _ecs.getComponent<KeybindButton_t>(entity);
        auto* dynText = _ecs.getComponent<DynamicText_t>(entity);
        
        if (!text || !keybind || !text->visible) continue;
        
        // Synchronise la touche actuelle depuis l'InputSystem (pour gérer les resets)
        keybind->currentKey = _inputSystem.getKeyForAction(keybind->action);
        
        std::string fullText = "";
        
        // Ajoute le préfixe (nom de l'action) si disponible
        if (dynText) {
            fullText = dynText->prefix;
        }
        
        // Ajoute le nom de la touche
        if (keybind->isWaitingForInput) {
            fullText += "Press a key...";
        } else {
            fullText += getKeyName(keybind->currentKey);
        }
        
        std::snprintf(text->text, sizeof(text->text), "%s", fullText.c_str());
    }
}

std::string MenuSystem::getKeyName(sf::Keyboard::Key key)
{
    switch (key) {
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::B: return "B";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::G: return "G";
        case sf::Keyboard::H: return "H";
        case sf::Keyboard::I: return "I";
        case sf::Keyboard::J: return "J";
        case sf::Keyboard::K: return "K";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::M: return "M";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::O: return "O";
        case sf::Keyboard::P: return "P";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::U: return "U";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::Y: return "Y";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::Num0: return "0";
        case sf::Keyboard::Num1: return "1";
        case sf::Keyboard::Num2: return "2";
        case sf::Keyboard::Num3: return "3";
        case sf::Keyboard::Num4: return "4";
        case sf::Keyboard::Num5: return "5";
        case sf::Keyboard::Num6: return "6";
        case sf::Keyboard::Num7: return "7";
        case sf::Keyboard::Num8: return "8";
        case sf::Keyboard::Num9: return "9";
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Return: return "Enter";
        case sf::Keyboard::Up: return "Up";
        case sf::Keyboard::Down: return "Down";
        case sf::Keyboard::Left: return "Left";
        case sf::Keyboard::Right: return "Right";
        case sf::Keyboard::Escape: return "Escape";
        case sf::Keyboard::LShift: return "LShift";
        case sf::Keyboard::RShift: return "RShift";
        case sf::Keyboard::LControl: return "LCtrl";
        case sf::Keyboard::RControl: return "RCtrl";
        default: return "Unknown";
    }
}
