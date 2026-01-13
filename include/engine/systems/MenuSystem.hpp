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
#include "engine/EntityFactory.hpp"
#include "engine/StageFactory.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>

class MenuSystem : public ISystem {
public:
    explicit MenuSystem(ECS& ecs);
    void update(double dt) override;
    void setSceneManager(std::unique_ptr<SceneManager> sceneManager);
    void registerAction(const std::string& actionId, std::function<void()> callback);
    void setEnabled(bool enabled);

private:
    void handleInput();
    void updateHighlight();
    void executeSelectedAction();
    std::vector<Entity> _menuItems;
    bool _actionTriggered = false;
    int _currentSelection = 0;
    std::unique_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, std::function<void()>> _actionCallbacks;
    bool _enabled = true;
};

MenuSystem::MenuSystem(ECS& ecs) : ISystem(ecs) {}

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
    (void)dt;
    if (!_enabled) return;
    _menuItems = _ecs.getEntitiesByComponents<Position_t, Text_t, MenuItem_t>();
    if (_menuItems.empty()) return;

    handleInput();
    updateHighlight();
    if (_actionTriggered) {
        executeSelectedAction();
        _actionTriggered = false;
    }
}

void MenuSystem::handleInput()
{
    _actionTriggered = false;

    // Ne gère les inputs que si MenuSystem est activé
    if (!_enabled) return;

    // Navigation dans le menu
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        int nextSelection = (_currentSelection + 1) % _menuItems.size();
        // Trouve le prochain élément visible
        while (nextSelection != _currentSelection) {
            Entity nextEntity = _menuItems[nextSelection];
            auto* text = _ecs.getComponent<Text_t>(nextEntity);
            if (text && text->visible) {
                _currentSelection = nextSelection;
                break;
            }
            nextSelection = (nextSelection + 1) % _menuItems.size();
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        int prevSelection = (_currentSelection - 1 + _menuItems.size()) % _menuItems.size();
        // Trouve l'élément visible précédent
        while (prevSelection != _currentSelection) {
            Entity prevEntity = _menuItems[prevSelection];
            auto* text = _ecs.getComponent<Text_t>(prevEntity);
            if (text && text->visible) {
                _currentSelection = prevSelection;
                break;
            }
            prevSelection = (prevSelection - 1 + _menuItems.size()) % _menuItems.size();
        }
    }

    // Détection de la touche pour déclencher une action
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && !_actionTriggered) {
        Entity selected = _menuItems[_currentSelection];
        auto* text = _ecs.getComponent<Text_t>(selected);
        if (text && text->visible) {  // Vérifie que l'élément sélectionné est visible
            _actionTriggered = true;
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
