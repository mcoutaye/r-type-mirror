/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** SceneManager
*/

#pragma once
#include <vector>
#include <string>
#include <unordered_map>

enum class SceneState { Active, Inactive };

struct Scene {
    std::string id;
    SceneState state;
    // Autres données spécifiques à la scène (ex: entités, musique, etc.)
};

class SceneManager {
public:
    void addScene(const std::string& id);
    void setActiveScene(const std::string& id);
    void deactivateScene(const std::string& id);
    SceneState getSceneState(const std::string& id) const;
    const std::string& getActiveSceneId() const;

private:
    std::vector<std::string> _sceneStack;  // Ordre des scènes (pour la superposition)
    std::unordered_map<std::string, Scene> _scenes;  // Scènes disponibles
    std::string _activeSceneId;  // Scène actuellement active
};

void SceneManager::addScene(const std::string& id) {
    _scenes[id] = Scene{id, SceneState::Inactive};
}

void SceneManager::setActiveScene(const std::string& id) {
    // Désactive la scène active précédente
    if (!_activeSceneId.empty()) {
        _scenes[_activeSceneId].state = SceneState::Inactive;
    }
    // Active la nouvelle scène
    _scenes[id].state = SceneState::Active;
    _activeSceneId = id;
    // Ajoute à la pile si elle n'y est pas déjà
    if (std::find(_sceneStack.begin(), _sceneStack.end(), id) == _sceneStack.end()) {
        _sceneStack.push_back(id);
    }
}

void SceneManager::deactivateScene(const std::string& id) {
    _scenes[id].state = SceneState::Inactive;
    if (_activeSceneId == id) {
        _activeSceneId.clear();
    }
}

SceneState SceneManager::getSceneState(const std::string& id) const {
    auto it = _scenes.find(id);
    return (it != _scenes.end()) ? it->second.state : SceneState::Inactive;
}

const std::string& SceneManager::getActiveSceneId() const {
    return _activeSceneId;
}
