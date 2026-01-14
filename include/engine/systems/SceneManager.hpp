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
};

class SceneManager {
public:
    void addScene(const std::string& id);
    void setActiveScene(const std::string& id);
    void deactivateScene(const std::string& id);
    SceneState getSceneState(const std::string& id) const;
    const std::string& getActiveSceneId() const;
    void goBack();

private:
    std::vector<std::string> _sceneStack;
    std::unordered_map<std::string, Scene> _scenes;
    std::string _activeSceneId;
};

void SceneManager::addScene(const std::string& id)
{
    _scenes[id] = Scene{id, SceneState::Inactive};
}

void SceneManager::setActiveScene(const std::string& id)
{
    if (_scenes.find(id) == _scenes.end()) return;

    if (!_activeSceneId.empty()) {
        _scenes[_activeSceneId].state = SceneState::Inactive;
    }

    if (!_activeSceneId.empty()) {
        _sceneStack.push_back(_activeSceneId);
    }

    _activeSceneId = id;
    _scenes[id].state = SceneState::Active;
}

void SceneManager::goBack()
{
    if (_sceneStack.empty()) return;

    if (!_activeSceneId.empty()) {
        _scenes[_activeSceneId].state = SceneState::Inactive;
    }

    _activeSceneId = _sceneStack.back();
    _sceneStack.pop_back();

    if (!_activeSceneId.empty()) {
        _scenes[_activeSceneId].state = SceneState::Active;
    }
}

void SceneManager::deactivateScene(const std::string& id)
{
    if (_scenes.find(id) != _scenes.end()) {
        _scenes[id].state = SceneState::Inactive;
    }
}

SceneState SceneManager::getSceneState(const std::string& id) const
{
    auto it = _scenes.find(id);
    if (it != _scenes.end()) {
        return it->second.state;
    }
    return SceneState::Inactive;
}

const std::string& SceneManager::getActiveSceneId() const
{
    return _activeSceneId;
}
