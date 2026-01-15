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
    int layer;
};

class SceneManager {
public:
    void addScene(const std::string& id, int layer = 0);
    void setActiveScene(const std::string& id);
    void setSceneVisibility(const std::string& id, bool visible);
    void setScenesVisibility(const std::vector<std::string>& sceneIds, bool visible);
    void toggleSceneVisibility(const std::string& id);
    void goBack(const std::string& fallbackScene);
    const std::vector<std::string>& getVisibleScenes() const;
    const std::string& getTopSceneId() const;
    SceneState getSceneState(const std::string& id) const;

private:
    std::vector<std::string> _sceneStack;
    std::unordered_map<std::string, Scene> _scenes;
    std::set<std::string> _visibleScenes;
};

void SceneManager::addScene(const std::string& id, int layer)
{
    _scenes[id] = Scene{id, SceneState::Inactive, layer};
}

void SceneManager::setActiveScene(const std::string& id)
{
    if (_scenes.find(id) == _scenes.end()) return;

    if (!_sceneStack.empty()) {
        _scenes[_sceneStack.back()].state = SceneState::Inactive;
    }

    if (!_sceneStack.empty()) {
        _sceneStack.push_back(_sceneStack.back());
    }
    _sceneStack.push_back(id);

    _scenes[id].state = SceneState::Active;
    _visibleScenes.insert(id);
}

void SceneManager::setSceneVisibility(const std::string& id, bool visible)
{
    if (_scenes.find(id) == _scenes.end()) return;

    if (visible) {
        _visibleScenes.insert(id);
        _scenes[id].state = SceneState::Active;
    } else {
        _visibleScenes.erase(id);
        _scenes[id].state = SceneState::Inactive;
    }
}

void SceneManager::setScenesVisibility(const std::vector<std::string>& sceneIds, bool visible)
{
    for (const auto& sceneId : sceneIds) {
        if (_scenes.find(sceneId) == _scenes.end()) continue;

        if (visible) {
            _visibleScenes.insert(sceneId);
            _scenes[sceneId].state = SceneState::Active;
        } else {
            _visibleScenes.erase(sceneId);
            _scenes[sceneId].state = SceneState::Inactive;
        }
    }
}

void SceneManager::toggleSceneVisibility(const std::string& id)
{
    if (_scenes.find(id) == _scenes.end()) return;

    if (_visibleScenes.find(id) != _visibleScenes.end()) {
        _visibleScenes.erase(id);
        _scenes[id].state = SceneState::Inactive;
    } else {
        _visibleScenes.insert(id);
        _scenes[id].state = SceneState::Active;
    }
}

void SceneManager::goBack(const std::string& fallbackScene)
{
    if (_scenes.find(fallbackScene) != _scenes.end()) {
        setActiveScene(fallbackScene);
        return;
    }

    std::string currentScene = _sceneStack.back();
    _visibleScenes.erase(currentScene);
    _scenes[currentScene].state = SceneState::Inactive;
    _sceneStack.pop_back();

    std::string previousScene = _sceneStack.back();
    _visibleScenes.insert(previousScene);
    _scenes[previousScene].state = SceneState::Active;
}


const std::vector<std::string>& SceneManager::getVisibleScenes() const
{
    static std::vector<std::string> visibleScenes;
    visibleScenes.assign(_visibleScenes.begin(), _visibleScenes.end());

    std::sort(visibleScenes.begin(), visibleScenes.end(), [this](const std::string& a, const std::string& b) {
        return _scenes.at(a).layer < _scenes.at(b).layer;
    });

    return visibleScenes;
}

const std::string& SceneManager::getTopSceneId() const
{
    static std::string empty;
    if (_visibleScenes.empty()) {
        return empty;
    }

    // Trouve la scène visible avec le layer le plus élevé
    const std::string* topScene = nullptr;
    int highestLayer = -1;

    for (const auto& sceneId : _visibleScenes) {
        auto it = _scenes.find(sceneId);
        if (it != _scenes.end() && it->second.layer > highestLayer) {
            highestLayer = it->second.layer;
            topScene = &sceneId;
        }
    }

    return topScene ? *topScene : empty;
}

SceneState SceneManager::getSceneState(const std::string& id) const
{
    auto it = _scenes.find(id);
    if (it != _scenes.end()) {
        return it->second.state;
    }
    return SceneState::Inactive;
}


