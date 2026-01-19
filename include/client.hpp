/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** client
*/

#pragma once
#include "ecs.hpp"
#include "UdpClient.hpp"
#include "Timer.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/MoveSystem.hpp"
#include "engine/systems/RessourceManager.hpp"
#include "engine/systems/SoundSystem.hpp"
#include "engine/systems/MenuSystem.hpp"
#include "engine/systems/SceneManager.hpp"
#include "engine/systems/ParticleSystem.hpp"
#include "engine/systems/CameraSystem.hpp"
#include <memory>
#include <iostream>
#include <stdexcept>

class Client {
public:
    Client(sf::IpAddress serverIp = "127.0.0.1");
    ~Client() = default;
    void update();
    void render();
    void processInput();

    bool _running = true;

private:
    void initializeMenus();
    void initializeGame();
    void onQuit();
    void onStartGame();
    void onOpenOptions();
    void onOpenAudio();
    void onOpenKeybindings();
    void onBackToOptions();
    void onResumeGame();
    void onBackToMainMenu();
    void applyUpdate(EntityUpdate &update);

    std::unique_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, std::vector<Entity>> _sceneEntities;
    std::unordered_map<std::size_t, std::size_t> serverToClientEntityRelation;
    float _shootCooldown = 0.f;
    Entity _localPlayerEntity = -1;

    ResourceManager& _resourceManager;
    ECS _ecs;
    sf::RenderWindow _window;
    sf::View _defaultView;  // Vue par défaut pour les menus
    Timer _timer;

    InputSystem _inputSystem{_ecs};
    RenderSystem _renderSystem{_ecs, _window, _resourceManager};
    MoveSystem _moveSystem{_ecs};
    SoundSystem _soundSystem{_ecs};
    MenuSystem _menuSystem{_ecs, _inputSystem};
    ParticleSystem _particleSystem{_ecs, 3000};
    CameraSystem _cameraSystem{_ecs, _window};

    bool _debugHitboxes = false;

    enum class GameState { Menu, InGame };
    GameState _gameState = GameState::Menu;
    bool _gameRunningInBackground = false;  // true si le jeu tourne en arrière-plan (pause)

    UdpClient _UDP;
};


Client::Client(sf::IpAddress serverIp)
    : _UDP(UdpClient(serverIp, SERVER_PORT)),
      _resourceManager(ResourceManager::getInstance()),
      _ecs(ECS())
{
    std::cout << "[Client] Création de la fenêtre..." << std::endl;
    
    // Sur certains environnements Windows, le fullscreen peut échouer
    // On essaie d'abord en fenêtré si le fullscreen ne fonctionne pas
    try {
        _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
        if (!_window.isOpen()) {
            std::cerr << "[Client] ATTENTION: Fullscreen a échoué, passage en fenêtré..." << std::endl;
            _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Default);
        }
    } catch (...) {
        std::cerr << "[Client] ERREUR: Impossible de créer la fenêtre en fullscreen, tentative en fenêtré..." << std::endl;
        _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Default);
    }
    
    if (!_window.isOpen()) {
        std::cerr << "[Client] ERREUR FATALE: Impossible de créer la fenêtre SFML!" << std::endl;
        _running = false;
        throw std::runtime_error("Impossible de créer la fenêtre SFML");
    }
    
    std::cout << "[Client] Fenêtre créée avec succès (" 
              << _window.getSize().x << "x" << _window.getSize().y << ")" << std::endl;
    
    _window.setFramerateLimit(60);
    _defaultView = _window.getDefaultView();  // Sauvegarde la vue par défaut
    
    std::cout << "[Client] Démarrage du client UDP..." << std::endl;
    _UDP.start();
    _timer = Timer();
    _running = true;

    // Initialize resources on client side for rendering
    _resourceManager.initialize();

    //     // === CRÉATION DU STAGE ===
    // Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);
    std::cout << "[Client] Initialisation du SceneManager..." << std::endl;
    _sceneManager = std::make_unique<SceneManager>();
    _sceneManager->addScene("main_menu");
    _sceneManager->addScene("options_menu");
    _sceneManager->addScene("audio_menu");
    _sceneManager->addScene("keybindings_menu");
    _sceneManager->addScene("game");
    _sceneManager->addScene("victory_menu");
    _sceneManager->addScene("defeat_menu");
    _menuSystem.setSceneManager(std::make_unique<SceneManager>(*_sceneManager));

    std::cout << "[Client] Chargement des ressources..." << std::endl;
    bool fontOk1 = _resourceManager.loadFont("default", "assets/font/Vipnagorgialla-Rg.ttf");
    bool fontOk2 = _resourceManager.loadFont("title", "assets/font/Rostex-Outline.ttf");
    
    if (!fontOk1 || !fontOk2) {
        std::cerr << "[Client] ATTENTION: Certaines polices n'ont pas pu être chargées!" << std::endl;
    }

    std::cout << "[Client] Chargement des sons..." << std::endl;
    // === CHARGEMENT DES SONS ===
    _soundSystem.loadSound("shoot.ogg", "assets/sound/shoot.ogg");
    _soundSystem.loadSound("enemy_death.ogg", "assets/sound/enemy_death.ogg");
    _soundSystem.loadSound("player_death.ogg", "assets/sound/player_death.ogg");
    _soundSystem.loadSound("block_destruction.ogg", "assets/sound/block_destruction.ogg");
    _soundSystem.loadSound("hit.ogg", "assets/sound/hit.ogg");
    _soundSystem.loadSound("click.ogg", "assets/sound/click.ogg");
    _soundSystem.loadMusic("menu_theme", "assets/music/menu.ogg");
    _soundSystem.loadMusic("game_theme", "assets/music/background.ogg");

    _menuSystem.registerAction("quit", [this]() { onQuit(); });
    _menuSystem.registerAction("start_game", [this]() { onStartGame(); });
    _menuSystem.registerAction("open_options", [this]() { onOpenOptions(); });
    _menuSystem.registerAction("open_audio", [this]() { onOpenAudio(); });
    _menuSystem.registerAction("open_keybindings", [this]() { onOpenKeybindings(); });
    _menuSystem.registerAction("resume_game", [this]() { onResumeGame(); });
    _menuSystem.registerAction("back_to_main_menu", [this]() { onBackToMainMenu(); });
    _menuSystem.registerAction("back_to_options", [this]() { onBackToOptions(); });
    _menuSystem.registerAction("reset_keybinds", [this]() { _inputSystem.resetToDefaultMappings(); });
    
    _menuSystem.setSoundSystem(&_soundSystem);
    _menuSystem.setWindow(&_window);

    std::cout << "[Client] Création de la caméra..." << std::endl;
    // Crée la caméra dès le début (pour qu'elle soit toujours disponible)
    Entity cameraEntity = _ecs.createEntity();
    Camera_t camera;
    camera.x = 960.f;
    camera.y = 540.f;
    camera.targetX = 960.f;
    camera.targetY = 540.f;
    camera.smoothSpeed = 3.f;
    camera.zoom = 1.f;
    camera.targetZoom = 1.f;
    camera.viewWidth = 1920.f;
    camera.viewHeight = 1080.f;
    camera.useBounds = false;
    _ecs.addComponent(cameraEntity, camera);

    std::cout << "[Client] Initialisation des menus..." << std::endl;
    initializeMenus();
    _sceneManager->setActiveScene("main_menu");
    
    std::cout << "[Client] Démarrage de la musique de menu..." << std::endl;
    Entity gameMusic = _ecs.createEntity();
    _ecs.addComponent(gameMusic, BackgroundMusic_t{"menu_theme", true, 40.f});
    
    std::cout << "[Client] Initialisation terminée avec succès!" << std::endl;
}

void Client::onQuit()
{
    _running = false;
}

void Client::onStartGame()
{
    _gameState = GameState::InGame;
    _gameRunningInBackground = false;
    _sceneManager->setActiveScene("game");
    _menuSystem.setEnabled(false);
    initializeGame();
}

void Client::onResumeGame()
{
    _gameRunningInBackground = false;
    _sceneManager->setActiveScene("game");
    _menuSystem.setEnabled(false);
}

void Client::onBackToMainMenu()
{
    // Si on est en jeu pausé, retourne au menu pause
    if (_gameRunningInBackground) {
        _sceneManager->setActiveScene("pause_menu");
        _menuSystem.setEnabled(true);
    } else {
        // Sinon, retourne vraiment au menu principal
        _gameState = GameState::Menu;
        _gameRunningInBackground = false;
        _sceneManager->setActiveScene("main_menu");
        _menuSystem.setEnabled(true);
    }
}

void Client::onOpenOptions()
{
    _sceneManager->setActiveScene("options_menu");
    _menuSystem.setEnabled(true);
}

void Client::onOpenAudio()
{
    _sceneManager->setActiveScene("audio_menu");
}

void Client::onOpenKeybindings()
{
    _sceneManager->setActiveScene("keybindings_menu");
}

void Client::onBackToOptions()
{
    _sceneManager->setActiveScene("options_menu");
}

void Client::initializeMenus()
{
    // Menu principal
    Menu mainMenu = {
        "main",
        {
            {"Play", {"start_game"}, 960.f, 400.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
            {"Settings", {"open_options"}, 960.f, 520.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
            {"Quit", {"quit"}, 960.f, 640.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
        },
        "R-TYPE",
        960.f, 200.f,
        "title", 120, sf::Color::Red,
        true,
        "menu_theme"
    };

    // Menu options
    Menu optionsMenu = {
        "options",
        {
            {"Audio", {"open_audio"}, 960.f, 400.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, true},
            {"Controls", {"open_keybindings"}, 960.f, 500.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, true},
            {"Back", {"back_to_main_menu"}, 960.f, 600.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, true}
        },
        "SETTINGS",
        960.f, 200.f,
        "title", 100, sf::Color::Blue,
        true,
        ""
    };

    // Menu pause
    Menu pauseMenu = {
        "pause",
        {
            {"Resume", {"resume_game"}, 960.f, 400.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false},
            {"Settings", {"open_options"}, 960.f, 480.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false},
            {"Quit", {"quit"}, 960.f, 560.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false}
        },
        "PAUSE",
        960.f, 300.f,
        "title", 100, sf::Color::Yellow,
        true,
        ""
    };

    // Menu audio
    Menu audioMenu = {
        "audio_menu",
        {
            {"Music Volume: 100%", {}, 960.f, 350.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"SFX Volume: 100%", {}, 960.f, 450.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Back", {"back_to_options"}, 960.f, 600.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, true}
        },
        "AUDIO SETTINGS",
        960.f, 200.f,
        "title", 100, sf::Color::Blue,
        true,
        ""
    };

    // Menu keybindings
    Menu keybindingsMenu = {
        "keybindings_menu",
        {
            {"Move Up: Up", {}, 960.f, 300.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Move Down: Down", {}, 960.f, 370.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Move Left: Left", {}, 960.f, 440.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Move Right: Right", {}, 960.f, 510.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Shoot: Space", {}, 960.f, 580.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Quit: Escape", {}, 960.f, 650.f, "default", 40, sf::Color::White, sf::Color::Green, 1.0f, true, true},
            {"Reset to Defaults", {"reset_keybinds"}, 960.f, 750.f, "default", 40, sf::Color::Cyan, sf::Color::Red, 1.1f, true, true},
            {"Back", {"back_to_options"}, 960.f, 820.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, true}
        },
        "KEYBINDINGS",
        960.f, 180.f,
        "title", 100, sf::Color::Blue,
        true,
        ""
    };

    Menu victoryMenu = {
        "victory",
        {
            {"Quit", {"quit"}, 960.f, 600.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, true}
        },
        "VICTORY",
        960.f, 200.f,
        "title", 100, sf::Color::Green,
        true,
        ""
    };

    Menu defeatMenu = {
        "game_over",
        {
            {"Quit", {"quit"}, 960.f, 600.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, true}
        },
        "GAME OVER",
        960.f, 200.f,
        "title", 100, sf::Color::Red,
        true,
        ""
    };


    // Création des entités pour le menu principal
    std::vector<Entity> mainMenuEntities;
    mainMenuEntities.push_back(Factory::createMenuTitle(_ecs, mainMenu));
    for (const auto& item : mainMenu.items) {
        mainMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["main_menu"] = mainMenuEntities;

    // Création des entités pour le menu options
    std::vector<Entity> optionsMenuEntities;
    optionsMenuEntities.push_back(Factory::createMenuTitle(_ecs, optionsMenu));
    for (const auto& item : optionsMenu.items) {
        optionsMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["options_menu"] = optionsMenuEntities;

    // Création des entités pour le menu pause
    std::vector<Entity> pauseMenuEntities;
    pauseMenuEntities.push_back(Factory::createMenuTitle(_ecs, pauseMenu));
    for (const auto& item : pauseMenu.items) {
        pauseMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["pause_menu"] = pauseMenuEntities;

    // Création des entités pour le menu audio
    std::vector<Entity> audioMenuEntities;
    audioMenuEntities.push_back(Factory::createMenuTitle(_ecs, audioMenu));
    
    // Music volume slider
    Entity musicSlider = Factory::createMenuItem(_ecs, audioMenu.items[0]);
    Slider_t musicSliderComp;
    musicSliderComp.minValue = 0.0f;
    musicSliderComp.maxValue = 100.0f;
    musicSliderComp.currentValue = _soundSystem.getMusicVolume();
    musicSliderComp.step = 5.0f;  // 5% par appui
    musicSliderComp.linkedSetting = "music_volume";
    _ecs.addComponent<Slider_t>(musicSlider, musicSliderComp);
    
    DynamicText_t musicDynText;
    musicDynText.prefix = "Music Volume: ";
    // Provide a value getter so the UI text reflects the slider's currentValue
    musicDynText.suffix = "%";
    musicDynText.valueGetter = [this, musicSlider]() {
        auto* s = _ecs.getComponent<Slider_t>(musicSlider);
        int val = s ? static_cast<int>(s->currentValue) : 0;
        return std::to_string(val);
    };
    _ecs.addComponent<DynamicText_t>(musicSlider, musicDynText);
    audioMenuEntities.push_back(musicSlider);
    
    // SFX volume slider
    Entity sfxSlider = Factory::createMenuItem(_ecs, audioMenu.items[1]);
    Slider_t sfxSliderComp;
    sfxSliderComp.minValue = 0.0f;
    sfxSliderComp.maxValue = 100.0f;
    sfxSliderComp.currentValue = _soundSystem.getSoundVolume();
    sfxSliderComp.step = 5.0f;  // 5% par appui
    sfxSliderComp.linkedSetting = "sfx_volume";
    _ecs.addComponent<Slider_t>(sfxSlider, sfxSliderComp);
    
    DynamicText_t sfxDynText;
    sfxDynText.prefix = "SFX Volume: ";
    // Provide a value getter so the UI text reflects the slider's currentValue
    sfxDynText.suffix = "%";
    sfxDynText.valueGetter = [this, sfxSlider]() {
        auto* s = _ecs.getComponent<Slider_t>(sfxSlider);
        int val = s ? static_cast<int>(s->currentValue) : 0;
        return std::to_string(val);
    };
    _ecs.addComponent<DynamicText_t>(sfxSlider, sfxDynText);
    audioMenuEntities.push_back(sfxSlider);
    
    // Back button
    audioMenuEntities.push_back(Factory::createMenuItem(_ecs, audioMenu.items[2]));
    _sceneEntities["audio_menu"] = audioMenuEntities;

    // Création des entités pour le menu keybindings
    std::vector<Entity> keybindingsMenuEntities;
    keybindingsMenuEntities.push_back(Factory::createMenuTitle(_ecs, keybindingsMenu));
    
    // Keybind buttons for each action
    Entity moveUpBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[0]);
    KeybindButton_t moveUpKeybind;
    moveUpKeybind.action = GameAction::MoveUp;
    moveUpKeybind.isWaitingForInput = false;
    moveUpKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::MoveUp);
    _ecs.addComponent<KeybindButton_t>(moveUpBtn, moveUpKeybind);
    
    DynamicText_t moveUpDynText;
    moveUpDynText.prefix = "Move Up: ";
    _ecs.addComponent<DynamicText_t>(moveUpBtn, moveUpDynText);
    keybindingsMenuEntities.push_back(moveUpBtn);
    
    Entity moveDownBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[1]);
    KeybindButton_t moveDownKeybind;
    moveDownKeybind.action = GameAction::MoveDown;
    moveDownKeybind.isWaitingForInput = false;
    moveDownKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::MoveDown);
    _ecs.addComponent<KeybindButton_t>(moveDownBtn, moveDownKeybind);
    
    DynamicText_t moveDownDynText;
    moveDownDynText.prefix = "Move Down: ";
    _ecs.addComponent<DynamicText_t>(moveDownBtn, moveDownDynText);
    keybindingsMenuEntities.push_back(moveDownBtn);
    
    Entity moveLeftBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[2]);
    KeybindButton_t moveLeftKeybind;
    moveLeftKeybind.action = GameAction::MoveLeft;
    moveLeftKeybind.isWaitingForInput = false;
    moveLeftKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::MoveLeft);
    _ecs.addComponent<KeybindButton_t>(moveLeftBtn, moveLeftKeybind);
    
    DynamicText_t moveLeftDynText;
    moveLeftDynText.prefix = "Move Left: ";
    _ecs.addComponent<DynamicText_t>(moveLeftBtn, moveLeftDynText);
    keybindingsMenuEntities.push_back(moveLeftBtn);
    
    Entity moveRightBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[3]);
    KeybindButton_t moveRightKeybind;
    moveRightKeybind.action = GameAction::MoveRight;
    moveRightKeybind.isWaitingForInput = false;
    moveRightKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::MoveRight);
    _ecs.addComponent<KeybindButton_t>(moveRightBtn, moveRightKeybind);
    
    DynamicText_t moveRightDynText;
    moveRightDynText.prefix = "Move Right: ";
    _ecs.addComponent<DynamicText_t>(moveRightBtn, moveRightDynText);
    keybindingsMenuEntities.push_back(moveRightBtn);
    
    Entity shootBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[4]);
    KeybindButton_t shootKeybind;
    shootKeybind.action = GameAction::Shoot;
    shootKeybind.isWaitingForInput = false;
    shootKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::Shoot);
    _ecs.addComponent<KeybindButton_t>(shootBtn, shootKeybind);
    
    DynamicText_t shootDynText;
    shootDynText.prefix = "Shoot: ";
    _ecs.addComponent<DynamicText_t>(shootBtn, shootDynText);
    keybindingsMenuEntities.push_back(shootBtn);
    
    Entity quitBtn = Factory::createMenuItem(_ecs, keybindingsMenu.items[5]);
    KeybindButton_t quitKeybind;
    quitKeybind.action = GameAction::Quit;
    quitKeybind.isWaitingForInput = false;
    quitKeybind.currentKey = _inputSystem.getKeyForAction(GameAction::Quit);
    _ecs.addComponent<KeybindButton_t>(quitBtn, quitKeybind);
    
    DynamicText_t quitDynText;
    quitDynText.prefix = "Quit: ";
    _ecs.addComponent<DynamicText_t>(quitBtn, quitDynText);
    keybindingsMenuEntities.push_back(quitBtn);
    
    // Reset and Back buttons (no special components)
    keybindingsMenuEntities.push_back(Factory::createMenuItem(_ecs, keybindingsMenu.items[6]));
    keybindingsMenuEntities.push_back(Factory::createMenuItem(_ecs, keybindingsMenu.items[7]));
    _sceneEntities["keybindings_menu"] = keybindingsMenuEntities;

    std::vector<Entity> victoryMenuEntities;
    victoryMenuEntities.push_back(Factory::createMenuTitle(_ecs, victoryMenu));
    for (const auto& item : victoryMenu.items) {
        victoryMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["victory_menu"] = victoryMenuEntities;

    std::vector<Entity> defeatMenuEntities;
    defeatMenuEntities.push_back(Factory::createMenuTitle(_ecs, defeatMenu));
    for (const auto& item : defeatMenu.items) {
        defeatMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["defeat_menu"] = defeatMenuEntities;
}


void Client::initializeGame()
{
    // Clean stars that already drifted off-screen before recreating the field
    Factory::destroyStarfield(_ecs, 1920.f, 1080.f, 50.f);

    // Crée le starfield pour le jeu
    Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);

    // Crée la musique du jeu
    Entity gameMusic = _ecs.createEntity();
    _ecs.addComponent(gameMusic, BackgroundMusic_t{"game_theme", true, 40.f});
}

void Client::update()
{
    _timer.updateClock();
    double dt = 1.0 / 60.0;  // 60 FPS fixe – tu peux utiliser _timer.getDeltaTime() si tu préfères variable

    // === INPUTS & MENU : toujours actifs (nécessaire pour naviguer dans le menu et quitter à tout moment) ===
    _inputSystem.update(dt);
    _menuSystem.update(dt);

    std::string activeSceneId = _sceneManager->getActiveSceneId();

    // ============================================================
    // KILL ENTITIES WITH NO HEALTH (client-side cleanup)
    // Server sends death notifications, but we also check locally
    // for responsiveness and to handle edge cases
    // ============================================================
    auto entitiesWithHealth = _ecs.getEntitiesByComponents<Health_t>();
    for (Entity e : entitiesWithHealth) {
        auto* health = _ecs.getComponent<Health_t>(e);
        if (health && health->current <= 0) {
            _ecs.killEntity(e);
            
            // Remove from mapping if exists
            for (auto it = serverToClientEntityRelation.begin(); it != serverToClientEntityRelation.end(); ++it) {
                if (it->second == e) {
                    serverToClientEntityRelation.erase(it);
                    break;
                }
            }
            
            // Reset local player entity reference if it was the player
            if (e == _localPlayerEntity) {
                _localPlayerEntity = -1;
            }
        }
    }

    // Remove entities out of bounds
    auto entities = _ecs.getEntitiesByComponents<Position_t>();
    for (Entity e : entities) {
        // Don't kill the local player if they go out of bounds (let server handle it or block movement)
        if (e == _localPlayerEntity) continue;

        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos && (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f)) {
            // Kill stars that go off-screen to prevent memory leak
            if (!_ecs.hasComponent<Star_t>(e)) {
                _ecs.killEntity(e);
            }
        }
    }

    // Destroy stars that go off-screen to prevent accumulation and MAX_ENTITIES assertion failure
    auto stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    for (Entity e : stars) {
        auto* pos = _ecs.getComponent<Position_t>(e);
        if (pos && pos->x < -50.f) {  // Star has left the screen
            _ecs.killEntity(e);
        }
    }
    
    // Recreate missing stars to maintain starfield count
    // This replaces old stars that went off-screen with new ones
    stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
    if (stars.size() < 150) {  // Target is 150 stars (from initializeGame)
        int starsToDeclare = 150 - stars.size();
        for (int i = 0; i < starsToDeclare; ++i) {
            float x = 1930.f;  // Spawn at right edge
            float y = static_cast<float>(rand() % 1080);
            Factory::createStar(_ecs, x, y, 10);  // Use same layer count as initializeGame
        }
    }

    // ============================================================
    // CLIENT-SIDE COLLISION: Visual prediction only
    // Remove projectiles that appear to hit targets for smoother visuals
    // Server handles actual damage and authoritative collision detection
    // ============================================================
    auto projectiles = _ecs.getEntitiesByComponents<Projectile_t, Position_t, Collider_t>();
    auto targets = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

    for (Entity p : projectiles) {
        auto projPos = _ecs.getComponent<Position_t>(p);
        auto projCol = _ecs.getComponent<Collider_t>(p);
        auto projData = _ecs.getComponent<Projectile_t>(p);

        if (!projPos || !projCol) continue;

        for (Entity t : targets) {
            if (p == t) continue;
            if (t == _localPlayerEntity) continue;  // Don't predict hits on local player
            
            // Skip projectile's owner (prevents projectile from disappearing immediately)
            if (projData && projData->ownerId >= 0 && projData->ownerId == (int)t) continue;
            
            if (_ecs.hasComponent<Projectile_t>(t)) continue;  // Don't check projectile vs projectile

            auto targetPos = _ecs.getComponent<Position_t>(t);
            auto targetCol = _ecs.getComponent<Collider_t>(t);

            if (!targetPos || !targetCol) continue;

            // Skip if projectile and target are on the same team (and not neutral team 0)
            if (projCol && projCol->team == targetCol->team && targetCol->team != 0) continue;

            // Center-based AABB check to match server CollisionSystem
            float left1 = projPos->x - projCol->width / 2.f;
            float right1 = projPos->x + projCol->width / 2.f;
            float top1 = projPos->y - projCol->height / 2.f;
            float bottom1 = projPos->y + projCol->height / 2.f;

            float left2 = targetPos->x - targetCol->width / 2.f;
            float right2 = targetPos->x + targetCol->width / 2.f;
            float top2 = targetPos->y - targetCol->height / 2.f;
            float bottom2 = targetPos->y + targetCol->height / 2.f;

            if (left1 < right2 && right1 > left2 &&
                top1 < bottom2 && bottom1 > top2) {

                // Visual prediction: remove projectile immediately for smooth gameplay
                // Server will send authoritative update about actual hit
                _ecs.killEntity(p);
                break;
            }
        }
    }

    // === TOGGLE PAUSE (fonctionne en jeu et dans le menu pause) ===
    if (_inputSystem.wasActionPressed(GameAction::Pause)) {
        if (activeSceneId == "game") {
            // En jeu → ouvre le menu pause
            _gameRunningInBackground = true;
            _sceneManager->setActiveScene("pause_menu");
            _menuSystem.setEnabled(true);  // Active la navigation menu
            activeSceneId = "pause_menu";
        } else if (activeSceneId == "pause_menu") {
            // Dans le menu pause → retourne au jeu
            _gameRunningInBackground = false;
            _sceneManager->setActiveScene("game");
            _menuSystem.setEnabled(false);  // Désactive la navigation menu
            activeSceneId = "game";
        }
    }

    for (auto& [sceneId, entities] : _sceneEntities) {
        bool isActive = (sceneId == activeSceneId);
        for (Entity e : entities) {
            if (auto* text = _ecs.getComponent<Text_t>(e)) {
                text->visible = isActive;
            }
            if (auto* drawable = _ecs.getComponent<Drawable_t>(e)) {
                drawable->visible = isActive;
            }
        }
    }

    // === GESTION DU MENU (seulement si pas en jeu pausé) ===
    if (!_gameRunningInBackground && (activeSceneId == "main_menu" || activeSceneId == "options_menu" || 
        activeSceneId == "audio_menu" || activeSceneId == "keybindings_menu")) {
        _soundSystem.update(dt);
        return;
    }

    // === MODE IN-GAME : le jeu continue de tourner (même si on est dans les menus de pause) ===
    if (activeSceneId == "game" || _gameRunningInBackground) {
        // 1. Réception et application des updates du serveur
        auto stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
        for (Entity e : stars) {
            auto* pos = _ecs.getComponent<Position_t>(e);
            auto* star = _ecs.getComponent<Star_t>(e);
            if (!pos || !star) continue;

            // Déplace l'étoile vers la gauche
            pos->x -= star->speed * dt;

            // Réapparition à droite si elle sort de l'écran
            if (pos->x < -10.f) {
                pos->x = 1930.f;
                pos->y = static_cast<float>(rand() % 1080);
            }
        }
        std::vector<EntityUpdate> updates;
        while (_UDP.receivedUpdates.tryPop(updates)) {
            for (auto& update : updates) {
                this->applyUpdate(update);
            }
        }
        
        // Mise à jour de la caméra (même avant le début du jeu, pour qu'elle suive le joueur)
        _cameraSystem.update(dt);

        // 2. Mise à jour des systèmes physiques
        _moveSystem.update(dt);

        // 3. Nettoyage des entités hors écran (sauf le joueur local)
        // Dans Client::update()
        auto entities = _ecs.getEntitiesByComponents<Position_t>();
        for (Entity e : entities) {
            if (e == _localPlayerEntity) continue;
            if (_ecs.hasComponent<Star_t>(e)) continue;  // <-- Ignore les étoiles !
            auto* pos = _ecs.getComponent<Position_t>(e);
            if (pos && (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f)) {
                _ecs.killEntity(e);
            }
        }

        // 4. Collisions client-side pour supprimer les projectiles immédiatement (meilleure réactivité visuelle)
        auto projectiles = _ecs.getEntitiesByComponents<Projectile_t, Position_t, Collider_t>();
        auto targets = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

        for (Entity p : projectiles) {
            auto* projPos = _ecs.getComponent<Position_t>(p);
            auto* projCol = _ecs.getComponent<Collider_t>(p);
            auto* projData = _ecs.getComponent<Projectile_t>(p);
            if (!projPos || !projCol) continue;

            for (Entity t : targets) {
                if (p == t) continue;
                if (t == _localPlayerEntity) continue;  // Le serveur gère les dommages sur le joueur
                
                // Skip projectile's owner (prevents projectile from disappearing immediately)
                if (projData && projData->ownerId >= 0 && projData->ownerId == (int)t) continue;
                
                // Skip projectile-to-projectile collisions (different teams can coexist)
                if (_ecs.hasComponent<Projectile_t>(t)) continue;

                auto* targetPos = _ecs.getComponent<Position_t>(t);
                auto* targetCol = _ecs.getComponent<Collider_t>(t);
                if (!targetPos || !targetCol) continue;

                // Skip if projectile and target are on the same team (and not neutral team 0)
                if (projCol && projCol->team == targetCol->team && targetCol->team != 0) continue;

                // Test AABB simple (center-based collision)
                float projLeft = projPos->x - projCol->width / 2.f;
                float projRight = projPos->x + projCol->width / 2.f;
                float projTop = projPos->y - projCol->height / 2.f;
                float projBottom = projPos->y + projCol->height / 2.f;

                float targetLeft = targetPos->x - targetCol->width / 2.f;
                float targetRight = targetPos->x + targetCol->width / 2.f;
                float targetTop = targetPos->y - targetCol->height / 2.f;
                float targetBottom = targetPos->y + targetCol->height / 2.f;

                if (projLeft < targetRight && projRight > targetLeft &&
                    projTop < targetBottom && projBottom > targetTop) {
                    _ecs.killEntity(p);
                    // Optionnel : jouer un son d'impact ici si tu veux un feedback immédiat
                    // Entity hitSound = _ecs.createEntity();
                    // _ecs.addComponent(hitSound, PlaySound_t{"hit.wav", 70.f});
                    break;
                }
            }
        }

        // 5. Mise à jour des sons (toujours en dernier, car ils peuvent être déclenchés par applyUpdate ou collisions)
        _soundSystem.update(dt);
        
        // 6. Mise à jour des particules
        _particleSystem.update(dt);
    }
}

void Client::applyUpdate(EntityUpdate &update)
{
    Entity entity = 0;

    if (serverToClientEntityRelation.find(update.entityId) == serverToClientEntityRelation.end()) {
        // Entity does not exist for client, create it
        entity = _ecs.createEntity();
        serverToClientEntityRelation[update.entityId] = entity;

        // Determine texture name based on entity type
        std::string textureName = "player";  // Default to player
        if (update.entityType == 1) {      // Enemy
            textureName = "enemy";
        } else if (update.entityType == 2) {  // Projectile
            textureName = "bullet";
        } else if (update.entityType == 3) {  // Destructible tile
            textureName = "block";
        }

        ResourceManager& rm = ResourceManager::getInstance();
        sf::IntRect spriteRect = rm.getSpriteRect(textureName);

        // Scale: shrink tiles visually; other entities keep current normalization
        float scale = 0.5f;            // Default for players/enemies/projectiles
        if (update.entityType == 3) {
            scale = 0.5f;             // Destructible tiles scaled down (was 1.0)
        }

        // Create Collider based on entity type - MUST match server-side dimensions
        _ecs.addComponents<Position_t, Health_t, Drawable_t, Collider_t>(entity,
            {update.position.x, update.position.y},
            {update.health.current, update.health.max},
            Drawable_t{textureName, {spriteRect}, 0, 0.1f, 0.0f, true, 10, true, scale, 0.f},
            (update.entityType == 0) ? Factory::createPlayerCollider() :
            (update.entityType == 1) ? Factory::createEnemyCollider() :
            (update.entityType == 2) ? Factory::createProjectileCollider(2, 0) :
            Factory::createTileCollider());

    } else {
        entity = serverToClientEntityRelation[update.entityId];
    }

    auto pos = _ecs.getComponent<Position_t>(entity);
    auto health = _ecs.getComponent<Health_t>(entity);

    if (pos) {
        pos->x = update.position.x;
        pos->y = update.position.y;
    }
    if (health) {
        health->current = update.health.current;
        health->max = update.health.max;

        if (health->current > health->max) {
            health->current = health->max;
        }

    }

    if (update.tick == MAGIC_TICK_LOCAL_PLAYER) {
        _localPlayerEntity = entity;
        // Ajoute le tag CameraTarget au joueur local pour que la caméra le suive
        if (!_ecs.hasComponent<CameraTarget_t>(entity)) {
            CameraTarget_t cameraTarget;
            cameraTarget.offsetX = 32.f;  // Centre sur le joueur (si sprite 64x64)
            cameraTarget.offsetY = 32.f;
            _ecs.addComponent(entity, cameraTarget);
        }
    } else if (update.tick == MAGIC_TICK_DEATH_OTHER || update.tick == MAGIC_TICK_DEATH_PLAYER) {
        // Entity died
        auto* deathPos = _ecs.getComponent<Position_t>(entity);
        float deathX = deathPos ? deathPos->x + 32.f : 0.f;
        float deathY = deathPos ? deathPos->y + 32.f : 0.f;
        
        _ecs.killEntity(entity);
        // Jouer un son d'explosion selon le type (approximation simple)
        if (_ecs.hasComponent<Projectile_t>(entity)) {
            // C'était un projectile → petit hit + sparks
            Entity hitSound = _ecs.createEntity();
            _ecs.addComponent(hitSound, Factory::createSound("hit.ogg", 70.f));
            _particleSystem.emitSparks(deathX, deathY, 10);
        } else {
            // Ennemi ou joueur → explosion
            Entity explosionSound = _ecs.createEntity();
            _ecs.addComponent(explosionSound, Factory::createSound("enemy_death.ogg", 90.f));
            _particleSystem.emitExplosion(deathX, deathY, 40);
            _particleSystem.emitDebris(deathX, deathY, 8);
            
            // Camera shake lors de l'explosion
            _cameraSystem.shake(15.f, 0.3f);  // intensity=15, duration=0.3s
        }

        if (serverToClientEntityRelation.find(update.entityId) != serverToClientEntityRelation.end()) {
            serverToClientEntityRelation.erase(update.entityId);
        }

        if (update.tick == MAGIC_TICK_DEATH_PLAYER) {
            std::cout << "I killed entity " << update.entityId << "!" << std::endl;
        }
    } else if (update.tick == MAGIC_TICK_SHOOT_PLAYER || update.tick == MAGIC_TICK_SHOOT_ENEMY) {
        // SHOOT message contains shooter's entity ID, so create projectile at shooter position
        if (entity != _localPlayerEntity) {  // Don't create duplicate for our own shots
            if (update.tick == MAGIC_TICK_SHOOT_PLAYER) {
                // Player shooting: projectile spawns slightly in front, moves right
                Entity projectile = Factory::createProjectile(_ecs,
                    update.position.x + 64.f, update.position.y + 20.f,
                    800.f, 0.f, 1, 25, "bullet",
                    entity, "shoot.ogg");  // Set ownerId to shooter's entity
                // Particules de tir joueur
                _particleSystem.emitTrail(update.position.x + 64.f, update.position.y + 20.f, 0.f, 5);
            } else {
                // Enemy shooting: projectile spawns in front, moves left
                Entity projectile = Factory::createProjectile(_ecs,
                    update.position.x - 20.f, update.position.y + 20.f,
                    -800.f, 0.f, 2, 25, "bullet",
                    entity, "shoot.ogg");  // Set ownerId to shooter's entity
                // Particules de tir ennemi (rouge)
                _particleSystem.emitCustom(update.position.x - 20.f, update.position.y + 20.f, 
                    5, 80.f, 30.f, 0.2f, 
                    sf::Color::Red, sf::Color(255, 100, 0, 0), 
                    4.f, 0.f, 0.f);
            }
        }
    }
    if (update.tick == MAGIC_TICK_VICTORY) {
        std::cout << "Victoire !" << std::endl;
        _sceneManager->setActiveScene("victory_menu");
        _menuSystem.setEnabled(true);
        _gameState = GameState::Menu;
    } else if (update.tick == MAGIC_TICK_DEFEAT) {
        std::cout << "Défaite..." << std::endl;
        _sceneManager->setActiveScene("defeat_menu");
        _menuSystem.setEnabled(true);
        _gameState = GameState::Menu;
    }
}

void Client::render()
{
    if (_debugHitboxes) _renderSystem.debugON(); else _renderSystem.debugOFF();
    _renderSystem.update(0); // dt is not used in render system
    std::string activeSceneId = _sceneManager->getActiveSceneId();
    
    // Clear en premier
    _window.clear(sf::Color::Black);
    
    // Applique la vue appropriée
    if (activeSceneId == "game") {
        // En jeu actif → vue caméra
        _cameraSystem.applyView();
    } else {
        // En pause ou dans un menu → vue par défaut (centrée sur l'écran)
        _window.setView(_defaultView);
    }
    
    _renderSystem.update(0);
    _particleSystem.render(_window);
    _window.display();
}

void Client::processInput()
{
    // IMPORTANT: Traite les événements SFML pour que l'OS ne considère pas l'app comme "ne répond pas"
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _running = false;
            return;
        }
        // Transmet les événements souris au MenuSystem
        _menuSystem.handleMouseInput(event);
    }

    // Ne traite les inputs de jeu que si on est en jeu (pas en pause/menu)
    std::string activeSceneId = _sceneManager->getActiveSceneId();
    if (activeSceneId != "game") {
        return;  // En pause ou dans un menu, pas d'inputs de jeu
    }

    InputState inputs = {0, 0, 0, 0, 0, 0};
    inputs.tick = _timer.getCurrentFrame();
    _inputSystem.update(0);

    // Client-only toggle for collider debug
    if (_inputSystem.wasActionPressed(GameAction::ToggleDebug)) {
        _debugHitboxes = !_debugHitboxes;
    }

    if (_inputSystem.isActionActive(GameAction::MoveUp))
        inputs.up = 1;
    if (_inputSystem.isActionActive(GameAction::MoveDown))
        inputs.down = 1;
    if (_inputSystem.isActionActive(GameAction::MoveLeft))
        inputs.left = 1;
    if (_inputSystem.isActionActive(GameAction::MoveRight))
        inputs.right = 1;
    if (_inputSystem.isActionActive(GameAction::Shoot)) {
        inputs.shoot = 1;
        if (_shootCooldown <= 0.f) {
            if (_localPlayerEntity != -1) {
                auto pos = _ecs.getComponent<Position_t>(_localPlayerEntity);
                if (pos) {
                    // Création du projectile + son
                    Factory::createProjectile(_ecs,
                        pos->x + 64.f, pos->y + 20.f,
                        800.f, 0.f,
                        1, 25, "bullet",
                        -1, "shoot.ogg");  // <-- le son est ajouté via la factory

                    // Particules de tir
                    _particleSystem.emitTrail(pos->x + 64.f, pos->y + 20.f, 0.f, 5);

                    _shootCooldown = SHOOT_DELAY;
                }
            }
        }
    }
    if (_shootCooldown > 0.f)
        _shootCooldown -= 1.0f / 60.f;
    _UDP.inputsToSend.push(inputs);
}
