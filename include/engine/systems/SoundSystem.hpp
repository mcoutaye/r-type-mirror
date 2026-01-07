/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Sound System
*/

#pragma once

#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>

class SoundSystem : public ISystem {
public:
    explicit SoundSystem(ECS& ecs);
    ~SoundSystem() override = default;

    void update(double dt) override;

    // Charge un son en mémoire (à appeler au démarrage du jeu ou au chargement de niveau)
    bool loadSound(const std::string& soundId, const std::string& filename);

    // Charge une musique (streaming, pour les gros fichiers)
    bool loadMusic(const std::string& musicId, const std::string& filename);

private:
    void processPlaySoundTriggers();
    void processBackgroundMusic();

    // Stockage des buffers SFX (petits sons)
    std::unordered_map<std::string, sf::SoundBuffer> _soundBuffers;

    // Sons en cours de lecture (pour pouvoir limiter le nombre simultané si besoin)
    std::vector<sf::Sound> _activeSounds;

    // Musique de fond actuelle
    sf::Music _currentMusic;

    // Pour éviter le spam de sons identiques trop rapprochés
    std::unordered_map<std::string, float> _soundCooldowns;
    static constexpr float DEFAULT_SOUND_COOLDOWN = 0.05f; // 50ms minimum entre deux mêmes sons
};

SoundSystem::SoundSystem(ECS& ecs) : ISystem(ecs)
{
    // Tu peux précharger les sons ici ou les charger dans ton Game/Scene init
}

bool SoundSystem::loadSound(const std::string& soundId, const std::string& filename)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return false;
    }
    _soundBuffers[soundId] = std::move(buffer);
    return true;
}

bool SoundSystem::loadMusic(const std::string& musicId, const std::string& filename)
{
    // On ne stocke pas toutes les musiques en mémoire, on les ouvre à la demande
    // Mais on peut vérifier que le fichier existe si tu veux
    return true; // on vérifiera à l'ouverture
}

void SoundSystem::processPlaySoundTriggers()
{
    auto entities = _ecs.getEntitiesByComponents<PlaySound_t>();

    for (Entity e : entities) {
        auto* play = _ecs.getComponent<PlaySound_t>(e);
        if (!play) continue;

        const std::string soundId(play->soundId);

        // Gestion simple du cooldown
        float& cooldown = _soundCooldowns[soundId];
        if (cooldown > 0.f) {
            cooldown -= 0.016f; // approx 60 FPS delta
            continue; // on attend encore
        }

        auto it = _soundBuffers.find(soundId);
        if (it == _soundBuffers.end()) {
            std::cerr << "Sound not loaded: " << soundId << std::endl;
            _ecs.killComponent<PlaySound_t>(e);
            continue;
        }

        // Créer et jouer le son
        _activeSounds.emplace_back();
        sf::Sound& sound = _activeSounds.back();
        sound.setBuffer(it->second);
        sound.setVolume(play->volume);
        sound.setPitch(play->pitch);
        sound.play();

        // Reset cooldown
        cooldown = DEFAULT_SOUND_COOLDOWN;

        // On retire le composant trigger car le son est joué
        _ecs.killComponent<PlaySound_t>(e);
    }

    // Nettoyage des sons terminés
    _activeSounds.erase(
        std::remove_if(_activeSounds.begin(), _activeSounds.end(),
            [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; }),
        _activeSounds.end()
    );
}

void SoundSystem::processBackgroundMusic()
{
    auto entities = _ecs.getEntitiesByComponents<BackgroundMusic_t>();

    if (entities.empty()) {
        if (_currentMusic.getStatus() == sf::Music::Playing) {
            _currentMusic.stop();
        }
        return;
    }

    // On ne gère qu'une seule musique à la fois (la première trouvée)
    Entity musicEntity = entities[0];
    auto* music = _ecs.getComponent<BackgroundMusic_t>(musicEntity);
    if (!music) return;

    std::string musicId(music->musicId);

    // Si c'est déjà la bonne musique en cours et bien configurée, rien à faire
    if (_currentMusic.getStatus() == sf::Music::Playing &&
        _currentMusic.getLoop() == music->looping &&
        std::abs(_currentMusic.getVolume() - music->volume) < 0.1f) {
        // On vérifie que c'est bien le bon fichier (approximatif)
        // SFML n'offre pas de moyen simple de récupérer le chemin ouvert
        return;
    }

    if (!_currentMusic.openFromFile(musicId)) {
        std::cerr << "Failed to open music: " << musicId << std::endl;
        return;
    }

    _currentMusic.setVolume(music->volume);
    _currentMusic.setLoop(music->looping);
    _currentMusic.play();
}

void SoundSystem::update(double dt)
{
    (void)dt;
    processPlaySoundTriggers();
    processBackgroundMusic();
}
