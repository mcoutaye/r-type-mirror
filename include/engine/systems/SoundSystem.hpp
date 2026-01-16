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

    // Contrôle du volume
    void setMusicVolume(float volume);   // 0.0f à 100.0f
    void setSoundVolume(float volume);   // 0.0f à 100.0f
    float getMusicVolume() const;
    float getSoundVolume() const;

private:
    void processPlaySoundTriggers();
    void processBackgroundMusic();

    // Stockage des buffers SFX (petits sons)
    std::unordered_map<std::string, sf::SoundBuffer> _soundBuffers;

    // Sons en cours de lecture (pour pouvoir limiter le nombre simultané si besoin)
    std::vector<sf::Sound> _activeSounds;

    // Pour éviter le spam de sons identiques trop rapprochés
    std::unordered_map<std::string, float> _soundCooldowns;
    static constexpr float DEFAULT_SOUND_COOLDOWN = 0.05f; // 50ms minimum entre deux mêmes sons
    std::unordered_map<std::string, std::string> _musicPaths;
    sf::Music _currentMusic;
    std::string _currentMusicId;
    
    float _musicVolume = 100.0f;
    float _soundVolume = 100.0f;
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
        sound.setVolume(play->volume * (_soundVolume / 100.0f));  // Applique le volume global
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

bool SoundSystem::loadMusic(const std::string& musicId, const std::string& filename)
{
    _musicPaths[musicId] = filename;
    return true;  // On vérifie l'existence du fichier plus tard
}

void SoundSystem::processBackgroundMusic() {
    auto entities = _ecs.getEntitiesByComponents<BackgroundMusic_t>();

    if (entities.empty()) {
        if (_currentMusic.getStatus() == sf::Music::Playing) {
            _currentMusic.stop();  // Arrête la musique actuelle s'il n'y a plus d'entité
            _currentMusicId.clear();
        }
        return;
    }

    // On ne gère qu'une seule musique à la fois (la première trouvée)
    Entity musicEntity = entities[0];
    auto* musicComp = _ecs.getComponent<BackgroundMusic_t>(musicEntity);
    if (!musicComp) return;

    std::string musicId(musicComp->musicId);

    // Si la musique demandée est différente de celle en cours
    if (_currentMusicId != musicId) {
        // Arrête la musique actuelle
        _currentMusic.stop();

        // Charge et joue la nouvelle musique
        auto it = _musicPaths.find(musicId);
        if (it == _musicPaths.end()) {
            std::cerr << "Music not loaded: " << musicId << std::endl;
            return;
        }

        if (!_currentMusic.openFromFile(it->second)) {
            std::cerr << "Failed to open music: " << it->second << std::endl;
            return;
        }

        _currentMusic.setVolume(musicComp->volume * (_musicVolume / 100.0f));  // Applique le volume global
        _currentMusic.setLoop(musicComp->looping);
        _currentMusic.play();
        _currentMusicId = musicId;
    }
}

void SoundSystem::setMusicVolume(float volume)
{
    _musicVolume = std::clamp(volume, 0.0f, 100.0f);
    if (_currentMusic.getStatus() != sf::Music::Stopped) {
        // Met à jour le volume de la musique en cours
        auto entities = _ecs.getEntitiesByComponents<BackgroundMusic_t>();
        if (!entities.empty()) {
            auto* musicComp = _ecs.getComponent<BackgroundMusic_t>(entities[0]);
            if (musicComp) {
                _currentMusic.setVolume(musicComp->volume * (_musicVolume / 100.0f));
            }
        }
    }
}

void SoundSystem::setSoundVolume(float volume)
{
    _soundVolume = std::clamp(volume, 0.0f, 100.0f);
}

float SoundSystem::getMusicVolume() const
{
    return _musicVolume;
}

float SoundSystem::getSoundVolume() const
{
    return _soundVolume;
}

void SoundSystem::update(double dt)
{
    (void)dt;
    processPlaySoundTriggers();
    processBackgroundMusic();
}
