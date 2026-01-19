/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** OptionsMenuSystem - Gère les sliders et le remapping de touches
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/SoundSystem.hpp"
#include <memory>

class OptionsMenuSystem : public ISystem {
public:
    explicit OptionsMenuSystem(ECS& ecs, InputSystem& inputSystem, SoundSystem& soundSystem);
    void update(double dt) override;
    void setEnabled(bool enabled);

private:
    void handleSliders();
    void handleKeybindButtons();
    void updateDynamicTexts();
    
    InputSystem& _inputSystem;
    SoundSystem& _soundSystem;
    bool _enabled = true;
    float _inputCooldown = 0.0f;
    const float _cooldownDuration = 0.15f;
    
    Entity _waitingForKeyEntity = static_cast<Entity>(-1);  // Entity en attente d'une nouvelle touche
};

OptionsMenuSystem::OptionsMenuSystem(ECS& ecs, InputSystem& inputSystem, SoundSystem& soundSystem) 
    : ISystem(ecs), _inputSystem(inputSystem), _soundSystem(soundSystem) {}

void OptionsMenuSystem::setEnabled(bool enabled)
{
    _enabled = enabled;
}

void OptionsMenuSystem::update(double dt)
{
    if (!_enabled) return;

    if (_inputCooldown > 0.0f) {
        _inputCooldown -= static_cast<float>(dt);
    }

    handleSliders();
    handleKeybindButtons();
    updateDynamicTexts();
}

void OptionsMenuSystem::handleSliders()
{
    auto sliders = _ecs.getEntitiesByComponents<Slider_t, Position_t, Text_t>();
    
    for (Entity e : sliders) {
        auto* slider = _ecs.getComponent<Slider_t>(e);
        auto* text = _ecs.getComponent<Text_t>(e);
        
        if (!slider || !text || !text->visible) continue;
        if (_inputCooldown > 0.0f) continue;

        bool leftPressed = _inputSystem.isActionActive(GameAction::MoveLeft);
        bool rightPressed = _inputSystem.isActionActive(GameAction::MoveRight);

        if (leftPressed) {
            slider->currentValue = std::max(slider->minValue, slider->currentValue - slider->step);
            _inputCooldown = _cooldownDuration;
            
            // Applique le changement selon le setting
            if (slider->linkedSetting == "music_volume") {
                _soundSystem.setMusicVolume(slider->currentValue);
            } else if (slider->linkedSetting == "sfx_volume") {
                _soundSystem.setSoundVolume(slider->currentValue);
            }
        } else if (rightPressed) {
            slider->currentValue = std::min(slider->maxValue, slider->currentValue + slider->step);
            _inputCooldown = _cooldownDuration;
            
            // Applique le changement selon le setting
            if (slider->linkedSetting == "music_volume") {
                _soundSystem.setMusicVolume(slider->currentValue);
            } else if (slider->linkedSetting == "sfx_volume") {
                _soundSystem.setSoundVolume(slider->currentValue);
            }
        }
    }
}

void OptionsMenuSystem::handleKeybindButtons()
{
    auto buttons = _ecs.getEntitiesByComponents<KeybindButton_t, Text_t>();
    
    // Si on attend une touche/bouton pour un bouton spécifique
    if (_waitingForKeyEntity != static_cast<Entity>(-1)) {
        auto* button = _ecs.getComponent<KeybindButton_t>(_waitingForKeyEntity);
        if (button && button->isWaitingForInput) {
            
            if (button->inputType == InputType::Keyboard) {
                // Écoute toutes les touches clavier
                for (int k = sf::Keyboard::A; k < sf::Keyboard::KeyCount; k++) {
                    sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(k);
                    if (sf::Keyboard::isKeyPressed(key) && key != sf::Keyboard::Escape) {
                        // Assigne la nouvelle touche
                        _inputSystem.setKeyMapping(key, button->action);
                        button->currentKey = key;
                        button->isWaitingForInput = false;
                        _waitingForKeyEntity = static_cast<Entity>(-1);
                        _inputCooldown = 0.3f;  // Cooldown plus long après remapping
                        break;
                    }
                }
            } else if (button->inputType == InputType::Joystick) {
                // Écoute tous les boutons de manette
                sf::Joystick::update();
                if (sf::Joystick::isConnected(0)) {
                    for (unsigned int btn = 0; btn < sf::Joystick::ButtonCount; btn++) {
                        if (sf::Joystick::isButtonPressed(0, btn)) {
                            // Assigne le nouveau bouton
                            _inputSystem.setJoystickButtonMapping(btn, button->action);
                            button->currentJoystickButton = btn;
                            button->isWaitingForInput = false;
                            _waitingForKeyEntity = static_cast<Entity>(-1);
                            _inputCooldown = 0.3f;
                            break;
                        }
                    }
                }
            }
            
            // Échap pour annuler
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                button->isWaitingForInput = false;
                _waitingForKeyEntity = static_cast<Entity>(-1);
                _inputCooldown = 0.3f;
            }
        }
        return;  // Ne traite pas d'autres inputs pendant le remapping
    }
    
    // Mode normal : clic sur un bouton pour commencer le remapping
    for (Entity e : buttons) {
        auto* button = _ecs.getComponent<KeybindButton_t>(e);
        auto* text = _ecs.getComponent<Text_t>(e);
        auto* menuItem = _ecs.getComponent<MenuItem_t>(e);
        
        if (!button || !text || !menuItem) continue;
        if (!text->visible || !menuItem->isSelected) continue;
        if (_inputCooldown > 0.0f) continue;

        bool confirmPressed = _inputSystem.isActionActive(GameAction::Shoot);
        
        if (confirmPressed) {
            button->isWaitingForInput = true;
            if (button->inputType == InputType::Keyboard) {
                button->currentKey = _inputSystem.getKeyForAction(button->action);
            } else {
                button->currentJoystickButton = _inputSystem.getJoystickButtonForAction(button->action);
            }
            _waitingForKeyEntity = e;
            _inputCooldown = 0.3f;
            break;
        }
    }
}

void OptionsMenuSystem::updateDynamicTexts()
{
    auto dynamicTexts = _ecs.getEntitiesByComponents<DynamicText_t, Text_t>();
    
    for (Entity e : dynamicTexts) {
        auto* dynText = _ecs.getComponent<DynamicText_t>(e);
        auto* text = _ecs.getComponent<Text_t>(e);
        
        if (!dynText || !text) continue;
        
        // Met à jour le texte avec la valeur dynamique
        if (dynText->valueGetter) {
            std::string newText = dynText->prefix + dynText->valueGetter() + dynText->suffix;
            std::snprintf(text->text, sizeof(text->text), "%s", newText.c_str());
        }
    }
    
    // Met à jour les textes des boutons de keybinding
    auto keybindButtons = _ecs.getEntitiesByComponents<KeybindButton_t, Text_t>();
    for (Entity e : keybindButtons) {
        auto* button = _ecs.getComponent<KeybindButton_t>(e);
        auto* text = _ecs.getComponent<Text_t>(e);
        
        if (!button || !text) continue;
        
        if (button->isWaitingForInput) {
            if (button->inputType == InputType::Keyboard) {
                std::snprintf(text->text, sizeof(text->text), "Press a key...");
            } else {
                std::snprintf(text->text, sizeof(text->text), "Press a button...");
            }
            text->color = sf::Color::Yellow;
        } else {
            std::string inputName;
            if (button->inputType == InputType::Keyboard) {
                sf::Keyboard::Key key = _inputSystem.getKeyForAction(button->action);
                inputName = getKeyName(key);
            } else {
                unsigned int btn = _inputSystem.getJoystickButtonForAction(button->action);
                inputName = getJoystickButtonName(btn);
            }
            std::snprintf(text->text, sizeof(text->text), "%s", inputName.c_str());
            text->color = text->originalColor;
        }
    }
}

// Fonction utilitaire pour obtenir le nom d'une touche
static std::string getKeyName(sf::Keyboard::Key key) {
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
        case sf::Keyboard::Up: return "Up Arrow";
        case sf::Keyboard::Down: return "Down Arrow";
        case sf::Keyboard::Left: return "Left Arrow";
        case sf::Keyboard::Right: return "Right Arrow";
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Enter: return "Enter";
        case sf::Keyboard::Escape: return "Escape";
        case sf::Keyboard::LControl: return "Left Ctrl";
        case sf::Keyboard::LShift: return "Left Shift";
        case sf::Keyboard::LAlt: return "Left Alt";
        case sf::Keyboard::RControl: return "Right Ctrl";
        case sf::Keyboard::RShift: return "Right Shift";
        case sf::Keyboard::RAlt: return "Right Alt";
        default: return "Unknown";
    }
}

// Fonction utilitaire pour obtenir le nom d'un bouton de manette
static std::string getJoystickButtonName(unsigned int button) {
    if (button == static_cast<unsigned int>(-1)) {
        return "None";
    }
    
    // Noms standards pour les manettes (type Xbox/PlayStation)
    switch (button) {
        case 0: return "Button A/X";
        case 1: return "Button B/Circle";
        case 2: return "Button X/Square";
        case 3: return "Button Y/Triangle";
        case 4: return "LB/L1";
        case 5: return "RB/R1";
        case 6: return "Back/Select";
        case 7: return "Start";
        case 8: return "L3";
        case 9: return "R3";
        default: return "Button " + std::to_string(button);
    }
}
