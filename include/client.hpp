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
#include <memory>

class Client {
public:
    Client(sf::IpAddress serverIp = "127.0.0.1");
    ~Client() = default;
    void update();
    void render();
    void processInput();
    void showScene(const std::string& id);
    void showScenes(const std::vector<std::string>& sceneIds);
    void hideScene(const std::string& id);
    void hideScenes(const std::vector<std::string>& sceneIds);
    void toggleScene(const std::string& id);

    Timer _timer;
    bool _running = true;

private:
    void initializeMenus();
    void initializeGame();
    void initializeHUD();
    void onQuit();
    void onStartGame();
    void onToggleOptions();
    void onBackToMainMenu();
    void onGoBack();
    void onToggleHUD();
    void onTogglePauseMenu();
    void onVictory();
    void onGameOver();
    void updateHUD();
    void applyUpdate(EntityUpdate &update);

    std::unique_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, std::vector<Entity>> _sceneEntities;
    std::unordered_map<std::size_t, std::size_t> serverToClientEntityRelation;
    float _shootCooldown = 0.f;
    Entity _localPlayerEntity = -1;

    ResourceManager& _resourceManager;
    ECS _ecs;
    sf::RenderWindow _window;

    InputSystem _inputSystem{_ecs};
    RenderSystem _renderSystem{_ecs, _window, _resourceManager};
    MoveSystem _moveSystem{_ecs};
    SoundSystem _soundSystem{_ecs};
    MenuSystem _menuSystem{_ecs};

    enum class GameState { Menu, InGame };
    GameState _gameState = GameState::Menu;

    UdpClient _UDP;
    int _score = 0;
    int _lives = 3;
};


Client::Client(sf::IpAddress serverIp)
    : _UDP(UdpClient(serverIp, SERVER_PORT)),
      _resourceManager(ResourceManager::getInstance()),
      _ecs(ECS())
{
    _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
    _UDP.start();
    _timer = Timer();
    _running = true;

    _sceneManager = std::make_unique<SceneManager>();
    _sceneManager->addScene("main_menu");
    _sceneManager->addScene("options_menu");
    _sceneManager->addScene("game");
    _sceneManager->addScene("pause_menu");
    _sceneManager->addScene("victory_menu");
    _sceneManager->addScene("game_over_menu");
    _menuSystem.setSceneManager(std::make_unique<SceneManager>(*_sceneManager));

    _resourceManager.loadFont("default", "assets/font/Vipnagorgialla-Rg.ttf");
    _resourceManager.loadFont("title", "assets/font/Rostex-Outline.ttf");

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
    _menuSystem.registerAction("toggle_options", [this]() { onToggleOptions(); });
    _menuSystem.registerAction("back_to_main_menu", [this]() { onBackToMainMenu(); });
    _menuSystem.registerAction("back", [this]() { onGoBack(); });
    _menuSystem.registerAction("toggle_hud", [this]() { onToggleHUD(); });
    _menuSystem.registerAction("toggle_pause", [this]() { onTogglePauseMenu(); });
    initializeMenus();
    showScene("main_menu");
    hideScenes({"game", "options_menu", "pause_menu", "victory_menu", "game_over_menu"});
    _soundSystem.setCurrentMusic("menu_theme", true, 50.f);
}

void Client::showScene(const std::string& id)
{
    _sceneManager->setSceneVisibility(id, true);
    _menuSystem.setEnabled(true);
}

void Client::showScenes(const std::vector<std::string>& sceneIds)
{
    _sceneManager->setScenesVisibility(sceneIds, true);
    _menuSystem.setEnabled(true);
}

void Client::hideScene(const std::string& id)
{
    _sceneManager->setSceneVisibility(id, false);
    if (_sceneManager->getTopSceneId() == "game") {
        _menuSystem.setEnabled(false);
    }
}

void Client::hideScenes(const std::vector<std::string>& sceneIds)
{
    _sceneManager->setScenesVisibility(sceneIds, false);
    _menuSystem.setEnabled(true);
    if (_sceneManager->getTopSceneId() == "game") {
        _menuSystem.setEnabled(false);
    }
}

void Client::toggleScene(const std::string& id)
{
    _sceneManager->toggleSceneVisibility(id);
    _menuSystem.setEnabled(!_sceneManager->getVisibleScenes().empty());
}

void Client::onQuit()
{
    _running = false;
}

void Client::onStartGame()
{
    hideScenes({"main_menu", "options_menu", "pause_menu", "victory_menu", "game_over_menu"});
    showScene("game");
    _soundSystem.setCurrentMusic("game_theme", true, 50.f);
    initializeGame();
}

void Client::onToggleOptions()
{
    hideScenes({"main_menu", "pause_menu", "victory_menu", "game_over_menu"});
    toggleScene("options_menu");
    if (_sceneManager->getSceneState("options_menu") == SceneState::Active) {
        _menuSystem.setEnabled(true);
    }
}

void Client::onBackToMainMenu()
{
    hideScenes({"options_menu", "pause_menu", "victory_menu", "game_over_menu"});
    showScene("main_menu");
    _soundSystem.setCurrentMusic("menu_theme", true, 50.f);
}

void Client::onVictory()
{
    hideScenes({"main_menu", "options_menu", "pause_menu", "game_over_menu"});
    showScene("victory_menu");
    _menuSystem.setEnabled(true);
}

void Client::onGameOver()
{
    hideScenes({"main_menu", "options_menu", "pause_menu", "victory_menu"});
    showScene("game_over_menu");
    _menuSystem.setEnabled(true);
}

void Client::onGoBack()
{
    if (_sceneManager->getSceneState("game") == SceneState::Active) {
        hideScenes({"main_menu", "options_menu", "victory_menu", "game_over_menu"});
        _sceneManager->goBack("pause_menu");
        _menuSystem.setEnabled(true);
    } else {
        hideScenes({"options_menu", "pause_menu", "victory_menu", "game_over_menu"});
        _sceneManager->goBack("main_menu");
        _menuSystem.setEnabled(true);
        _soundSystem.setCurrentMusic("menu_theme", true, 50.f);
    }
}

void Client::onToggleHUD()
{
    hideScenes({"main_menu", "options_menu", "pause_menu", "victory_menu", "game_over_menu"});
    showScene("hud");
}

void Client::onTogglePauseMenu()
{
    hideScenes({"main_menu", "options_menu", "victory_menu", "game_over_menu"});
    toggleScene("pause_menu");

    if (_sceneManager->getSceneState("pause_menu") == SceneState::Active) {
        _menuSystem.setEnabled(true);
    }
}


void Client::initializeMenus()
{
    Menu mainMenu = {
        "main",
        {
            {"Play", {"start_game"}, 960.f, 400.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
            {"Settings", {"toggle_options"}, 960.f, 520.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
            {"Quit", {"quit"}, 960.f, 640.f, "default", 64, sf::Color::White, sf::Color::Yellow, 1.2f, true, true},
        },
        "R-TYPE",
        960.f, 200.f,
        "title", 120, sf::Color::Red,
        true,
        "menu_theme"
    };

    Menu optionsMenu = {
        "options",
        {
            {"Audio", {""}, 960.f, 400.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, false},
            {"Back", {"back"}, 960.f, 500.f, "default", 50, sf::Color::White, sf::Color::Green, 1.1f, true, false}
        },
        "SETTINGS",
        960.f, 200.f,
        "title", 100, sf::Color::Blue,
        true,
        ""
    };

    Menu pauseMenu = {
        "pause",
        {
            {"Resume", {"toggle_pause"}, 960.f, 400.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false},
            {"Settings", {"toggle_options"}, 960.f, 480.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false},
            {"Quit", {"quit"}, 960.f, 560.f, "default", 50, sf::Color::White, sf::Color::Yellow, 1.1f, true, false}
        },
        "PAUSE",
        960.f, 300.f,
        "title", 100, sf::Color::Yellow,
        true,
        ""
    };

    Menu victoryMenu = {
        "victory",
        {
            {"Quit", {"quit"}, 960.f, 600.f, "default", 50, sf::Color::Green, sf::Color::Yellow, 1.1f, true, true}
        },
        "VICTORY",
        960.f, 300.f,
        "title", 100, sf::Color::Green,
        true,
        ""
    };

    Menu gameOverMenu = {
        "game_over",
        {
            {"Quit", {"quit"}, 960.f, 600.f, "default", 50, sf::Color::Red, sf::Color::Yellow, 1.1f, true, true}
        },
        "GAME OVER",
        960.f, 300.f,
        "title", 100, sf::Color::Red,
        true,
        ""
    };

    std::vector<Entity> mainMenuEntities;
    mainMenuEntities.push_back(Factory::createMenuTitle(_ecs, mainMenu));
    for (const auto& item : mainMenu.items) {
        mainMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["main_menu"] = mainMenuEntities;

    std::vector<Entity> optionsMenuEntities;
    optionsMenuEntities.push_back(Factory::createMenuTitle(_ecs, optionsMenu));
    for (const auto& item : optionsMenu.items) {
        optionsMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["options_menu"] = optionsMenuEntities;

    std::vector<Entity> pauseMenuEntities;
    pauseMenuEntities.push_back(Factory::createMenuTitle(_ecs, pauseMenu));
    for (const auto& item : pauseMenu.items) {
        pauseMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["pause_menu"] = pauseMenuEntities;

    std::vector<Entity> victoryMenuEntities;
    victoryMenuEntities.push_back(Factory::createMenuTitle(_ecs, victoryMenu));
    for (const auto& item : victoryMenu.items) {
        victoryMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["victory_menu"] = victoryMenuEntities;

    std::vector<Entity> gameOverMenuEntities;
    gameOverMenuEntities.push_back(Factory::createMenuTitle(_ecs, gameOverMenu));
    for (const auto& item : gameOverMenu.items) {
        gameOverMenuEntities.push_back(Factory::createMenuItem(_ecs, item));
    }
    _sceneEntities["game_over_menu"] = gameOverMenuEntities;
}

void Client::initializeHUD()
{
    Entity scoreEntity = Factory::createTextEntity(_ecs, "Score: 0", 50, 50, "default", 32, sf::Color::White, false);
    Entity livesEntity = Factory::createTextEntity(_ecs, "Vies: 3", 50, 100, "default", 32, sf::Color::White, false);

    _sceneEntities["hud"] = {scoreEntity, livesEntity};
}

void Client::initializeGame()
{
    Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);

    initializeHUD();

    Entity gameMusic = _ecs.createEntity();
    _ecs.addComponent(gameMusic, BackgroundMusic_t{"game_theme", true, 40.f});
}

void Client::updateHUD()
{
    for (Entity e : _sceneEntities["hud"]) {
        if (auto* text = _ecs.getComponent<Text_t>(e)) {
            if (std::string(text->text).find("Score:") != std::string::npos) {
                std::string scoreText = "Score: " + std::to_string(_score);
                std::memset(text->text, 0, sizeof(text->text));
                std::strncpy(text->text, scoreText.c_str(), sizeof(text->text) - 1);
            }
            else if (std::string(text->text).find("Vies:") != std::string::npos) {
                std::string livesText = "Vies: " + std::to_string(_lives);
                std::memset(text->text, 0, sizeof(text->text));
                std::strncpy(text->text, livesText.c_str(), sizeof(text->text) - 1);
            }
        }
    }
}

void Client::update()
{
    _timer.updateClock();
    double dt = 1.0 / 60.0;

    _inputSystem.update(dt);
    _menuSystem.update(dt);

    const std::vector<std::string>& visibleScenes = _sceneManager->getVisibleScenes();

    for (auto& [sceneId, entities] : _sceneEntities) {
        bool isVisible = std::find(visibleScenes.begin(), visibleScenes.end(), sceneId) != visibleScenes.end();
        for (Entity e : entities) {
            if (auto* text = _ecs.getComponent<Text_t>(e)) {
                text->visible = isVisible;
            }
            if (auto* drawable = _ecs.getComponent<Drawable_t>(e)) {
                drawable->visible = isVisible;
            }
        }
    }

    bool isGameActive = _sceneManager->getSceneState("game") == SceneState::Active;

    if (isGameActive) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
            onTogglePauseMenu();
            return;
        }

        auto stars = _ecs.getEntitiesByComponents<Star_t, Position_t>();
        for (Entity e : stars) {
            auto* pos = _ecs.getComponent<Position_t>(e);
            auto* star = _ecs.getComponent<Star_t>(e);
            if (!pos || !star) continue;

            pos->x -= star->speed * dt;

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

        _moveSystem.update(dt);

        auto entities = _ecs.getEntitiesByComponents<Position_t>();
        for (Entity e : entities) {
            if (e == _localPlayerEntity) continue;
            if (_ecs.hasComponent<Star_t>(e)) continue;
            auto* pos = _ecs.getComponent<Position_t>(e);
            if (pos && (pos->x < -100.f || pos->x > 2200.f || pos->y < -100.f || pos->y > 1200.f)) {
                _ecs.killEntity(e);
            }
        }

        auto projectiles = _ecs.getEntitiesByComponents<Projectile_t, Position_t, Collider_t>();
        auto targets = _ecs.getEntitiesByComponents<Position_t, Collider_t>();

        for (Entity p : projectiles) {
            auto* projPos = _ecs.getComponent<Position_t>(p);
            auto* projCol = _ecs.getComponent<Collider_t>(p);
            if (!projPos || !projCol) continue;

            for (Entity t : targets) {
                if (p == t) continue;
                if (t == _localPlayerEntity) continue;
                if (_ecs.hasComponent<Projectile_t>(t)) continue;

                auto* targetPos = _ecs.getComponent<Position_t>(t);
                auto* targetCol = _ecs.getComponent<Collider_t>(t);
                if (!targetPos || !targetCol) continue;

                if (projPos->x < targetPos->x + targetCol->width &&
                    projPos->x + projCol->width > targetPos->x &&
                    projPos->y < targetPos->y + targetCol->height &&
                    projPos->y + projCol->height > targetPos->y) {
                    _ecs.killEntity(p);
                    break;
                }
            }
        }
        updateHUD();
        _soundSystem.update(dt);
    }
}

void Client::applyUpdate(EntityUpdate &update)
{
    Entity entity = 0;

    _timer.updateClock();
    std::vector<EntityUpdate> updates;
    while (_UDP.receivedUpdates.tryPop(updates)) {
        for (auto &update : updates)
            this->applyUpdate(update);
    }

    _moveSystem.update(1.0f / 60.f);
    _soundSystem.update(1.0f / 60.f);

    if (serverToClientEntityRelation.find(update.entityId) == serverToClientEntityRelation.end()) {
        entity = _ecs.createEntity();
        serverToClientEntityRelation[update.entityId] = entity;

        _ecs.addComponents<Position_t, Health_t, Drawable_t, Collider_t>(entity,
            {update.position.x, update.position.y},
            {update.health.current, update.health.max},
            {"", sf::IntRect(0, 0, 64, 64), 10, true, 1.f, 0.f},
            Collider_t{64.f, 64.f, true, 2, 0});

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
    }

    if (update.tick == MAGIC_TICK_LOCAL_PLAYER) {
        _localPlayerEntity = entity;
    } else if (update.tick == MAGIC_TICK_DEATH_OTHER || update.tick == MAGIC_TICK_DEATH_PLAYER) {
        _ecs.killEntity(entity);
        if (_ecs.hasComponent<Projectile_t>(entity)) {
            Entity hitSound = _ecs.createEntity();
            _ecs.addComponent(hitSound, Factory::createSound("hit.ogg", 70.f));
        } else {
            Entity explosionSound = _ecs.createEntity();
            _ecs.addComponent(explosionSound, Factory::createSound("enemy_death.ogg", 90.f));
        }

        if (serverToClientEntityRelation.find(update.entityId) != serverToClientEntityRelation.end()) {
            serverToClientEntityRelation.erase(update.entityId);
        }

        if (update.tick == MAGIC_TICK_DEATH_PLAYER) {
            std::cout << "I killed entity " << update.entityId << "!" << std::endl;
        }
    } else if (update.tick == MAGIC_TICK_SHOOT_PLAYER || update.tick == MAGIC_TICK_SHOOT_ENEMY) {
        if (entity != _localPlayerEntity) {
            if (update.tick == MAGIC_TICK_SHOOT_PLAYER) {
                Factory::createProjectile(_ecs,
                    update.position.x + 64.f, update.position.y + 20.f,
                    800.f, 0.f, 1, 25, "",
                    -1, "shoot.ogg");
            } else {
                Factory::createProjectile(_ecs,
                    update.position.x - 20.f, update.position.y + 20.f,
                    -800.f, 0.f, 2, 25, "",
                    -1, "shoot.ogg");
            }
        }
    }
}

void Client::render()
{
    _renderSystem.update(0);
}

void Client::processInput()
{
    if (_sceneManager->getSceneState("options_menu") == SceneState::Active || _sceneManager->getSceneState("pause_menu") == SceneState::Active) {
        return;
    }

    InputState inputs = {0, 0, 0, 0, 0, 0};
    inputs.tick = _timer.getCurrentFrame();
    _inputSystem.update(0);
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
                    Factory::createProjectile(_ecs,
                        pos->x + 64.f, pos->y + 20.f,
                        800.f, 0.f,
                        1, 25, "",
                        -1, "shoot.ogg");

                    _shootCooldown = SHOOT_DELAY;
                }
            }
        }
    }
    if (_shootCooldown > 0.f)
        _shootCooldown -= 1.0f / 60.f;

    if (_inputSystem.isActionActive(GameAction::Quit)) {
        inputs.tick = MAGIC_TICK_CLIENT_QUIT;
        _running = false;
    }
    _UDP.inputsToSend.push(inputs);
}
