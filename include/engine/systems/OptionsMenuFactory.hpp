/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** OptionsMenuFactory - Crée les éléments de menu d'options (sliders, keybind buttons)
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/SoundSystem.hpp"

class OptionsMenuFactory {
public:
    // Crée un slider pour contrôler une valeur (volume, etc.)
    static Entity createSlider(ECS& ecs, float x, float y, 
                               const std::string& label,
                               float minValue, float maxValue, float currentValue,
                               const std::string& linkedSetting,
                               const std::string& fontId = "default")
    {
        Entity slider = ecs.createEntity();
        
        // Position
        ecs.addComponent(slider, Position_t{x, y});
        
        // Slider component
        Slider_t sliderComp;
        sliderComp.minValue = minValue;
        sliderComp.maxValue = maxValue;
        sliderComp.currentValue = currentValue;
        sliderComp.step = (maxValue - minValue) / 100.0f;  // 100 steps par défaut
        sliderComp.linkedSetting = linkedSetting;
        ecs.addComponent(slider, sliderComp);
        
        // Texte (label + valeur)
        Text_t text;
        text.content = label;
        text.font = fontId;
        text.characterSize = 40;
        text.color = sf::Color::White;
        text.originalColor = sf::Color::White;
        text.visible = true;
        ecs.addComponent(slider, text);
        
        // MenuItem pour la sélection
        MenuAction action;
        action.id = "slider_" + linkedSetting;
        ecs.addComponent(slider, MenuItem_t{action, false});
        
        // Highlight
        Highlight_t highlight;
        highlight.selectedColor = sf::Color::Cyan;
        highlight.selectedScale = 1.1f;
        ecs.addComponent(slider, highlight);
        
        // DynamicText pour afficher la valeur
        DynamicText_t dynText;
        dynText.prefix = label + ": ";
        dynText.suffix = "%";
        dynText.valueGetter = [&ecs, slider]() -> std::string {
            auto* sliderComp = ecs.getComponent<Slider_t>(slider);
            if (sliderComp) {
                return std::to_string(static_cast<int>(sliderComp->currentValue));
            }
            return "0";
        };
        ecs.addComponent(slider, dynText);
        
        return slider;
    }
    
    // Crée un bouton pour remapper une touche
    static Entity createKeybindButton(ECS& ecs, float x, float y,
                                     const std::string& label,
                                     GameAction action,
                                     InputSystem& inputSystem,
                                     const std::string& fontId = "default")
    {
        Entity button = ecs.createEntity();
        
        // Position
        ecs.addComponent(button, Position_t{x, y});
        
        // KeybindButton component
        KeybindButton_t keybindComp;
        keybindComp.action = action;
        keybindComp.isWaitingForInput = false;
        keybindComp.currentKey = inputSystem.getKeyForAction(action);
        ecs.addComponent(button, keybindComp);
        
        // Texte
        Text_t text;
        text.content = label + ": " + getKeyName(keybindComp.currentKey);
        text.font = fontId;
        text.characterSize = 36;
        text.color = sf::Color::White;
        text.originalColor = sf::Color::White;
        text.visible = true;
        ecs.addComponent(button, text);
        
        // MenuItem pour la sélection
        MenuAction menuAction;
        menuAction.id = "keybind_" + std::to_string(static_cast<int>(action));
        ecs.addComponent(button, MenuItem_t{menuAction, false});
        
        // Highlight
        Highlight_t highlight;
        highlight.selectedColor = sf::Color::Yellow;
        highlight.selectedScale = 1.1f;
        ecs.addComponent(button, highlight);
        
        return button;
    }
    
    // Crée un titre pour une section du menu
    static Entity createSectionTitle(ECS& ecs, float x, float y,
                                     const std::string& title,
                                     const std::string& fontId = "title")
    {
        Entity titleEntity = ecs.createEntity();
        
        ecs.addComponent(titleEntity, Position_t{x, y});
        
        Text_t text;
        text.content = title;
        text.font = fontId;
        text.characterSize = 60;
        text.color = sf::Color::Cyan;
        text.originalColor = sf::Color::Cyan;
        text.visible = true;
        ecs.addComponent(titleEntity, text);
        
        return titleEntity;
    }

private:
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
            case sf::Keyboard::Up: return "↑";
            case sf::Keyboard::Down: return "↓";
            case sf::Keyboard::Left: return "←";
            case sf::Keyboard::Right: return "→";
            case sf::Keyboard::Space: return "Space";
            case sf::Keyboard::Enter: return "Enter";
            case sf::Keyboard::Escape: return "Esc";
            case sf::Keyboard::LControl: return "LCtrl";
            case sf::Keyboard::LShift: return "LShift";
            case sf::Keyboard::LAlt: return "LAlt";
            default: return "?";
        }
    }
};
