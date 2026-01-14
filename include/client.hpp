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

    Timer _timer;
    bool _running = true;

private:
    void initializeMenus();
    void initializeGame();
    void onQuit();
    void onStartGame();
    void onOpenOptions();
    void onResumeGame();
    void onBackToMainMenu();
    void onVictory();
    void onGameOver();
    void onGoBack();
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
    _menuSystem.registerAction("open_options", [this]() { onOpenOptions(); });
    _menuSystem.registerAction("resume_game", [this]() { onResumeGame(); });
   _menuSystem.registerAction("back", [this]() { onGoBack(); });
    initializeMenus();
    _sceneManager->setActiveScene("main_menu");
    Entity gameMusic = _ecs.createEntity();
    _ecs.addComponent(gameMusic, BackgroundMusic_t{"menu_theme", true, 40.f});
}

void Client::onQuit()
{
    _running = false;
}

void Client::onStartGame()
{
    _gameState = GameState::InGame;
    _sceneManager->setActiveScene("game");
    _menuSystem.setEnabled(false);
    initializeGame();
}

void Client::onResumeGame()
{
    _sceneManager->setActiveScene("game");
    _menuSystem.setEnabled(false);
}

void Client::onBackToMainMenu()
{
    _sceneManager->setActiveScene("main_menu");
    _menuSystem.setEnabled(true);
}

void Client::onOpenOptions()
{
    _sceneManager->setActiveScene("options_menu");
    _menuSystem.setEnabled(true);
}

void Client::onVictory()
{
    _sceneManager->setActiveScene("victory_menu");
    _menuSystem.setEnabled(true);
}

void Client::onGameOver()
{
    _sceneManager->setActiveScene("game_over_menu");
    _menuSystem.setEnabled(true);
}

void Client::onGoBack()
{
    _sceneManager->goBack();
}

void Client::initializeMenus()
{
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


void Client::initializeGame()
{
    Factory::createStarfield(_ecs, 150, 1920.f, 1080.f, 10);

    Entity gameMusic = _ecs.createEntity();
    _ecs.addComponent(gameMusic, BackgroundMusic_t{"game_theme", true, 40.f});
}

void Client::update()
{
    _timer.updateClock();
    double dt = 1.0 / 60.0;

    _inputSystem.update(dt);
    _menuSystem.update(dt);

    std::string activeSceneId = _sceneManager->getActiveSceneId();

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

    if (activeSceneId == "main_menu" || activeSceneId == "options_menu" ||
        activeSceneId == "pause_menu" || activeSceneId == "victory_menu" || activeSceneId == "game_over_menu") {
        _soundSystem.update(dt);
        return;
    }

    if (activeSceneId == "game") {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
            _sceneManager->setActiveScene("pause_menu");
            _menuSystem.setEnabled(true);
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
