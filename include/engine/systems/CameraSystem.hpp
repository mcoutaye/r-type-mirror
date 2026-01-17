/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** CameraSystem - Gère la caméra 2D avec smooth follow, zoom et shake
*/

#pragma once
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <random>
#include <algorithm>

class CameraSystem : public ISystem {
public:
    explicit CameraSystem(ECS& ecs, sf::RenderWindow& window);
    void update(double dt) override;
    
    // Applique la vue de la caméra à la fenêtre
    void applyView();
    
    // Obtient la vue SFML de la caméra
    const sf::View& getView() const { return _view; }
    
    // Fonctions utilitaires
    void shake(float intensity, float duration);     // Déclenche un shake
    void setZoom(float zoom);                        // Change le zoom immédiatement
    void setTargetZoom(float zoom);                  // Change le zoom progressivement
    void setBounds(float minX, float minY, float maxX, float maxY);  // Définit les limites
    void removeBounds();                             // Enlève les limites
    
    // Conversion coordonnées écran <-> monde
    sf::Vector2f screenToWorld(const sf::Vector2i& screenPos) const;
    sf::Vector2i worldToScreen(const sf::Vector2f& worldPos) const;

private:
    void updateCameraFollow(Camera_t& camera, float dt);
    void updateCameraShake(Camera_t& camera, float dt);
    void updateCameraZoom(Camera_t& camera, float dt);
    void applyBounds(Camera_t& camera);
    
    sf::RenderWindow& _window;
    sf::View _view;
    
    // Pour le shake
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist{-1.f, 1.f};
};

// ============= IMPLEMENTATION =============

CameraSystem::CameraSystem(ECS& ecs, sf::RenderWindow& window)
    : ISystem(ecs), _window(window), _rng(std::random_device{}())
{
    _view = window.getDefaultView();
}

void CameraSystem::update(double dt)
{
    float fdt = static_cast<float>(dt);
    
    // Récupère la caméra (normalement il n'y en a qu'une)
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    
    if (cameras.empty()) return;
    
    Entity cameraEntity = cameras[0];
    auto* camera = _ecs.getComponent<Camera_t>(cameraEntity);
    
    if (!camera) return;
    
    // 1. Trouve la cible à suivre
    auto targets = _ecs.getEntitiesByComponents<CameraTarget_t, Position_t>();
    
    if (!targets.empty()) {
        // Prend la première cible trouvée
        Entity targetEntity = targets[0];
        auto* targetPos = _ecs.getComponent<Position_t>(targetEntity);
        auto* targetTag = _ecs.getComponent<CameraTarget_t>(targetEntity);
        
        if (targetPos && targetTag) {
            // Met à jour la position cible de la caméra
            camera->targetX = targetPos->x + targetTag->offsetX;
            camera->targetY = targetPos->y + targetTag->offsetY;
        }
    }
    
    // 2. Smooth follow
    updateCameraFollow(*camera, fdt);
    
    // 3. Zoom progressif
    updateCameraZoom(*camera, fdt);

    // 4. Shake effect
    updateCameraShake(*camera, fdt);
    
    // 5. Applique les limites du monde
    if (camera->useBounds) {
        applyBounds(*camera);
    }
    
    // 6. Met à jour la vue SFML
    float finalX = camera->x + camera->shakeOffsetX;
    float finalY = camera->y + camera->shakeOffsetY;
    
    _view.setCenter(finalX, finalY);
    _view.setSize(camera->viewWidth / camera->zoom, camera->viewHeight / camera->zoom);
}

void CameraSystem::updateCameraFollow(Camera_t& camera, float dt)
{
    // Interpolation linéaire (lerp) vers la cible
    float lerpFactor = 1.f - std::exp(-camera.smoothSpeed * dt);
    
    camera.x += (camera.targetX - camera.x) * lerpFactor;
    camera.y += (camera.targetY - camera.y) * lerpFactor;
}

void CameraSystem::updateCameraZoom(Camera_t& camera, float dt)
{
    // Interpolation vers le zoom cible
    float lerpFactor = 1.f - std::exp(-camera.zoomSpeed * dt);
    
    camera.zoom += (camera.targetZoom - camera.zoom) * lerpFactor;
    
    // Clamp le zoom pour éviter des valeurs extrêmes
    camera.zoom = std::clamp(camera.zoom, 0.1f, 10.f);
}

void CameraSystem::updateCameraShake(Camera_t& camera, float dt)
{
    if (camera.shakeDuration > 0.f) {
        camera.shakeDuration -= dt;
        
        // Génère un offset aléatoire basé sur l'intensité
        camera.shakeOffsetX = _dist(_rng) * camera.shakeIntensity;
        camera.shakeOffsetY = _dist(_rng) * camera.shakeIntensity;
        
        // Réduit progressivement l'intensité
        camera.shakeIntensity *= std::exp(-5.f * dt);
        
        if (camera.shakeDuration <= 0.f) {
            camera.shakeIntensity = 0.f;
            camera.shakeOffsetX = 0.f;
            camera.shakeOffsetY = 0.f;
        }
    }
}

void CameraSystem::applyBounds(Camera_t& camera)
{
    // Calcule les limites en tenant compte du zoom
    float halfWidth = (camera.viewWidth / camera.zoom) * 0.5f;
    float halfHeight = (camera.viewHeight / camera.zoom) * 0.5f;
    
    // Empêche la caméra de sortir des limites
    camera.x = std::clamp(camera.x, camera.minX + halfWidth, camera.maxX - halfWidth);
    camera.y = std::clamp(camera.y, camera.minY + halfHeight, camera.maxY - halfHeight);
    
    // Si les limites sont plus petites que la vue, centre la caméra
    if (camera.maxX - camera.minX < camera.viewWidth / camera.zoom) {
        camera.x = (camera.minX + camera.maxX) * 0.5f;
    }
    if (camera.maxY - camera.minY < camera.viewHeight / camera.zoom) {
        camera.y = (camera.minY + camera.maxY) * 0.5f;
    }
}

void CameraSystem::applyView()
{
    _window.setView(_view);
}

void CameraSystem::shake(float intensity, float duration)
{
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    if (cameras.empty()) return;
    
    auto* camera = _ecs.getComponent<Camera_t>(cameras[0]);
    if (!camera) return;
    
    camera->shakeIntensity = intensity;
    camera->shakeDuration = duration;
}

void CameraSystem::setZoom(float zoom)
{
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    if (cameras.empty()) return;
    
    auto* camera = _ecs.getComponent<Camera_t>(cameras[0]);
    if (!camera) return;
    
    camera->zoom = std::clamp(zoom, 0.1f, 10.f);
    camera->targetZoom = camera->zoom;
}

void CameraSystem::setTargetZoom(float zoom)
{
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    if (cameras.empty()) return;
    
    auto* camera = _ecs.getComponent<Camera_t>(cameras[0]);
    if (!camera) return;
    
    camera->targetZoom = std::clamp(zoom, 0.1f, 10.f);
}

void CameraSystem::setBounds(float minX, float minY, float maxX, float maxY)
{
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    if (cameras.empty()) return;
    
    auto* camera = _ecs.getComponent<Camera_t>(cameras[0]);
    if (!camera) return;
    
    camera->useBounds = true;
    camera->minX = minX;
    camera->minY = minY;
    camera->maxX = maxX;
    camera->maxY = maxY;
}

void CameraSystem::removeBounds()
{
    auto cameras = _ecs.getEntitiesByComponents<Camera_t>();
    if (cameras.empty()) return;
    
    auto* camera = _ecs.getComponent<Camera_t>(cameras[0]);
    if (!camera) return;
    
    camera->useBounds = false;
}

sf::Vector2f CameraSystem::screenToWorld(const sf::Vector2i& screenPos) const
{
    return _window.mapPixelToCoords(screenPos, _view);
}

sf::Vector2i CameraSystem::worldToScreen(const sf::Vector2f& worldPos) const
{
    return _window.mapCoordsToPixel(worldPos, _view);
}
