# 🎮 Game Engine Documentation

## Table des Matières
1. [Vue d'ensemble](#vue-densemble)
2. [Architecture ECS](#architecture-ecs)
3. [Composants](#composants)
4. [Systèmes](#systèmes)
5. [Factories](#factories)
6. [Resource Manager](#resource-manager)
7. [Guide d'utilisation](#guide-dutilisation)
8. [Exemples](#exemples)

---

## Vue d'ensemble

Ce moteur de jeu est construit sur une architecture **ECS (Entity Component System)** moderne et performante. Il est conçu pour être:
- **Modulaire**: Ajoutez ou retirez facilement des systèmes
- **Performant**: Architecture orientée données avec cache-friendly design
- **Réutilisable**: Facilement adaptable pour différents types de jeux
- **Type-safe**: Utilisation intensive des templates C++ pour la sécurité de type

### Caractéristiques principales
- Support jusqu'à **32,768 entités** simultanées
- Jusqu'à **128 composants** différents par entité
- Système de rendu avec layers et animations
- Gestion des collisions AABB
- Système de particules avec pooling
- Gestion audio (sons et musique)
- Support manette et clavier
- Scene management
- Physics system avec gravité

---

## Architecture ECS

### Concepts de base

#### Entity (Entité)
```cpp
using Entity = std::uint32_t; // Simple ID numérique
```
Une entité est simplement un **ID unique**. Elle n'a pas de logique ou de données propres.

#### Component (Composant)
Les composants sont des **structures de données pures** (POD - Plain Old Data). Ils ne contiennent que des données, pas de logique.

#### System (Système)
Les systèmes contiennent la **logique du jeu**. Ils opèrent sur les entités qui possèdent certains composants.

### Classe ECS principale

```cpp
class ECS {
public:
    // Gestion des entités
    Entity createEntity();
    void killEntity(Entity e);
    
    // Gestion des composants
    template<typename ComponentType>
    Entity addComponent(Entity e, const ComponentType &component);
    
    template<typename... ComponentType>
    Entity addComponents(Entity e, ComponentType&&... Components);
    
    template<typename ComponentType>
    bool hasComponent(Entity e) const;
    
    template<typename ComponentType>
    ComponentType* getComponent(Entity e);
    
    // Requêtes
    template<typename... ComponentTypes>
    std::vector<Entity> getEntitiesByComponents() const;
};
```

### Limites du système
```cpp
static constexpr size_t MAX_ENTITIES = 1 << 15;    // 32,768 entités max
static constexpr size_t MAX_COMPONENT = 1 << 7;    // 128 composants max
```

---

## Composants

Tous les composants sont définis dans `include/engine/systems/Components.hpp`.

### Composants de base

#### Position & Mouvement
```cpp
typedef struct Position_s {
    float x = 0.f;
    float y = 0.f;
} Position_t;

typedef struct Velocity_s {
    float x = 0.f;
    float y = 0.f;
} Velocity_t;
```

### Composants visuels

#### Drawable - Rendu avec animation
```cpp
typedef struct Drawable_s {
    std::string textureName;              // Nom de la texture
    std::vector<sf::IntRect> frames;      // Frames d'animation
    size_t currentFrameIndex = 0;         // Frame actuelle
    float animationSpeed = 0.1f;          // Temps entre frames
    float frameTimer = 0.0f;              // Timer interne
    bool loop = true;                     // Animation en boucle
    int layer = 0;                        // Ordre de rendu (0=arrière-plan, 50=premier plan)
    bool visible = true;
    float scale = 1.f;
    float rotation = 0.f;
} Drawable_t;
```

**Layers recommandés:**
- `0` : Background (étoiles, décor)
- `5` : Obstacles statiques
- `10` : Joueurs
- `15` : Ennemis
- `20` : Projectiles
- `50` : Particules/Effets

### Composants de gameplay

#### PlayerController
```cpp
typedef struct PlayerController_s {
    uint8_t playerId = 0;           // ID du joueur (0, 1, 2...)
    bool isShooting = false;
    float shootCooldown = 0.f;
} PlayerController_t;
```

#### Collider - Collision AABB
```cpp
typedef struct Collider_s {
    float width = 32.f;
    float height = 32.f;
    bool solid = true;              // Bloque le mouvement
    uint8_t team = 0;               // 0=neutre, 1=joueur, 2=ennemi
    int damage = 1;
} Collider_t;
```

#### Health - Points de vie
```cpp
typedef struct Health_s {
    int max = 100;
    int current = 100;
    int lastAttackerId = -1;
    float contactCooldown = 0.f;    // Cooldown entre dégâts de contact
} Health_t;
```

#### Projectile
```cpp
typedef struct Projectile_s {
    float speed;
    int damage;
    int ownerId = -1;               // Entity qui a tiré
} Projectile_t;
```

### Composants de physique

#### RigidBody - Corps physique
```cpp
typedef struct RigidBody_s {
    float mass = 1.f;
    float drag = 0.f;                   // Résistance de l'air
    float gravityScale = 1.f;
    bool useGravity = true;
    bool isKinematic = false;           // Si true, n'est pas affecté par les forces
} RigidBody_t;
```

#### Jumper - Capacité de saut
```cpp
typedef struct Jumper_s {
    float jumpForce = 500.f;
    float coyoteTime = 0.1f;            // Temps de grâce après avoir quitté le sol
    float jumpBufferTime = 0.1f;        // Buffer pour anticiper le saut
    float coyoteCounter = 0.f;
    float jumpBufferCounter = 0.f;
    bool isGrounded = false;
    bool canDoubleJump = false;
    bool hasDoubleJumped = false;
} Jumper_t;
```

### Composants de mouvement

#### MovementPattern - Patterns de mouvement IA
```cpp
typedef struct MovementPattern_s {
    enum class Type { 
        Linear,      // Ligne droite
        Sinus,       // Sinusoïde
        Cosinus,     // Cosinusoïde
        Circle,      // Cercle
        Zigzag,      // Zigzag
        Spiral       // Spirale
    };
    Type type;
    float amplitude;    // Pour les mouvements ondulatoires
    float frequency;    // Pour les mouvements ondulatoires
    float radius;       // Pour cercle/spirale
    float speed;        // Pour zigzag/spirale
} MovementPattern_t;
```

### Composants audio

#### PlaySound - Jouer un son
```cpp
typedef struct PlaySound_s {
    char soundId[64];           // "player_shoot.wav", "explosion.wav"
    float volume = 100.f;       // 0-100
    float pitch = 1.0f;         // Modification de tonalité
} PlaySound_t;
```

#### BackgroundMusic - Musique de fond
```cpp
typedef struct BackgroundMusic_s {
    char musicId[64];           // "level1.ogg", "menu_theme.ogg"
    bool looping = true;
    float volume = 50.f;
} BackgroundMusic_t;
```

### Composants de particules

#### ParticleEmitter - Émetteur de particules
```cpp
typedef struct ParticleEmitter_s {
    bool active = true;
    bool burst = false;                 // true=tout d'un coup, false=continu
    ParticleType type = ParticleType::Default;
    int particlesPerBurst = 20;
    float spawnRate = 10.f;             // Particules/seconde en mode continu
    float spawnAccumulator = 0.f;
    float emitterLifetime = -1.f;       // -1 = infini
    
    // Paramètres des particules
    float particleSpeed = 100.f;
    float particleSpread = 360.f;       // Angle de dispersion
    float particleLifetime = 1.f;
    sf::Color startColor = sf::Color::White;
    sf::Color endColor = sf::Color::Transparent;
    float startSize = 5.f;
    float endSize = 0.f;
    float particleGravity = 0.f;
} ParticleEmitter_t;
```

### Composants UI & Menu

#### Button - Bouton de menu
```cpp
typedef struct Button_s {
    ButtonType type = ButtonType::Custom;
    std::string text = "";
    std::function<void()> callback = nullptr;
    bool isHovered = false;
    bool isPressed = false;
    float width = 200.f;
    float height = 50.f;
} Button_t;
```

---

## Systèmes

### RenderSystem - Système de rendu

Gère le rendu visuel avec support d'animations et de layers.

```cpp
class RenderSystem : public ISystem {
public:
    RenderSystem(ECS& ecs, sf::RenderWindow& window, ResourceManager& rm);
    void update(double dt) override;
    void debugON();   // Active les colliders en rouge
    void debugOFF();  // Désactive le debug
};
```

**Fonctionnalités:**
- Tri automatique par layer
- Support d'animations multi-frames
- Mode debug pour visualiser les colliders
- Rendu des étoiles de fond (parallax)
- Gestion de la rotation et du scale

**Utilisation:**
```cpp
RenderSystem renderSystem(ecs, window, resourceManager);
renderSystem.update(deltaTime);
```

### InputSystem - Système d'entrées

Gère les entrées clavier et manette avec remapping.

```cpp
class InputSystem : public ISystem {
public:
    InputSystem(ECS& ecs);
    void update(double dt) override;
    
    // Vérification d'actions
    bool isActionActive(GameAction action) const;
    bool wasActionPressed(GameAction action) const;
    bool wasActionReleased(GameAction action) const;
    
    // Remapping clavier
    void setKeyMapping(sf::Keyboard::Key key, GameAction action);
    
    // Remapping manette
    void setJoystickButtonMapping(unsigned int button, GameAction action);
    
    // Enregistrer des fonctions pour les actions
    void registerActionFunction(GameAction action, 
                                std::function<void(Entity, double)> function);
};
```

**Actions disponibles:**
```cpp
enum class GameAction {
    MoveUp, MoveDown, MoveLeft, MoveRight,
    Shoot, Quit, Pause,
    MenuUp, MenuDown, MenuSelect,
    ToggleDebug
};
```

**Mapping par défaut:**
- Flèches directionnelles / ZQSD : Mouvement
- Espace : Tir
- Escape / P : Pause
- Enter : Sélection menu
- F1 : Toggle debug

### PhysicsSystem - Système de physique

Gère la gravité, les forces et le mouvement physique.

```cpp
class PhysicsSystem : public ISystem {
public:
    explicit PhysicsSystem(ECS& ecs, float gravity = 980.f);
    void update(double dt) override;
    
    void setGravity(float gravity);
    void applyGravity(Entity entity, float dt);
    void applyForce(Entity entity, sf::Vector2f force);
    void applyImpulse(Entity entity, sf::Vector2f impulse);
    bool isGrounded(Entity entity);
};
```

**Fonctionnalités:**
- Gravité configurable
- Application de forces et impulsions
- Détection du sol (grounded check)
- Support du drag (résistance de l'air)
- Séparation des axes X et Y pour les collisions

### CollisionSystem - Système de collision

Détection et résolution de collisions AABB.

```cpp
class CollisionSystem : public ISystem {
public:
    CollisionSystem(ECS& ecs);
    void update(double dt) override;
};
```

**Fonctionnalités:**
- Collision AABB centrée
- Système de teams (pas de dégâts entre alliés)
- Gestion des projectiles
- Support des tuiles destructibles
- Application automatique des dégâts

**Logique de collision:**
- Les entités de la même team (≠0) ne se font pas de dégâts
- Les projectiles sont détruits au contact
- Les dégâts sont appliqués via le composant Health

### MovementSystem - Patterns de mouvement IA

Gère les mouvements automatiques pour les ennemis.

```cpp
class MovementSystem : public ISystem {
public:
    MovementSystem(ECS& ecs);
    void update(double dt) override;
};
```

**Patterns disponibles:**
- **Linear**: Mouvement en ligne droite
- **Sinus**: Vague sinusoïdale
- **Cosinus**: Vague cosinusoïdale
- **Circle**: Mouvement circulaire
- **Zigzag**: Mouvement en zigzag
- **Spiral**: Spirale

### ParticleSystem - Système de particules

Système de particules haute performance avec pooling.

```cpp
class ParticleSystem : public ISystem {
public:
    explicit ParticleSystem(ECS& ecs, size_t poolSize = 2000);
    void update(double dt) override;
    void render(sf::RenderWindow& window);
    
    // Presets
    void emitExplosion(float x, float y, int count = 30);
    void emitSparks(float x, float y, int count = 15);
    void emitSmoke(float x, float y, int count = 10);
    void emitDebris(float x, float y, int count = 8);
    void emitTrail(float x, float y, float directionDeg, int count = 3);
    
    // Custom
    void emitCustom(float x, float y, int count, float speed, float spread,
                    float lifetime, sf::Color startColor, sf::Color endColor,
                    float startSize, float endSize, float gravity = 0.f);
};
```

**Fonctionnalités:**
- Pool de particules pré-alloué (pas d'allocation dynamique)
- Interpolation de couleur et taille
- Gravité par particule
- Effets prédéfinis (explosion, étincelles, fumée, etc.)

### SoundSystem - Système audio

Gestion des effets sonores et de la musique.

```cpp
class SoundSystem : public ISystem {
public:
    explicit SoundSystem(ECS& ecs);
    void update(double dt) override;
    
    bool loadSound(const std::string& soundId, const std::string& filename);
    bool loadMusic(const std::string& musicId, const std::string& filename);
    
    void setMusicVolume(float volume);  // 0-100
    void setSoundVolume(float volume);  // 0-100
};
```

**Fonctionnalités:**
- Chargement et stockage des sons en mémoire
- Streaming de musique pour les gros fichiers
- Cooldown anti-spam de sons
- Contrôle du volume et du pitch
- Gestion automatique des sons actifs

### WaveSystem - Système de vagues

Gestion des vagues d'ennemis (pour shoot'em up).

```cpp
class WaveSystem : public ISystem {
public:
    WaveSystem(ECS& ecs);
    void update(double dt) override;
    void loadWaveFile(const std::string& filename);
};
```

**Format de fichier de vagues:**
```
delay enemyType movementType count x y
2.0 enemy1 1 5 1800 200
3.0 enemy2 2 3 1800 500
```

### MenuSystem - Système de menus

Gestion des menus et de l'interface utilisateur.

```cpp
class MenuSystem : public ISystem {
public:
    MenuSystem(ECS& ecs);
    void update(double dt) override;
    void handleInput(const sf::Event& event);
};
```

### CameraSystem - Système de caméra

Gestion de la vue et du suivi de caméra.

```cpp
class CameraSystem : public ISystem {
public:
    CameraSystem(ECS& ecs, sf::RenderWindow& window);
    void update(double dt) override;
    void setTarget(Entity entity);
    void setZoom(float zoom);
    sf::View& getView();
};
```

---

## Factories

Les factories simplifient la création d'entités complexes.

### EntityFactory

Contient des fonctions helper pour créer des entités courantes.

#### Créer un joueur
```cpp
Entity createPlayer(ECS& ecs, float x, float y, uint8_t playerId, 
                   const std::string& textureName)
{
    Entity player = ecs.createEntity();
    ecs.addComponents<Position_t, Velocity_t, PlayerController_t, 
                     Drawable_t, Collider_t, Health_t, SendUpdate_t>
        (player,
            Position_t{x, y},
            Velocity_t{0.f, 0.f},
            PlayerController_t{playerId, false},
            Factory::createDrawable(textureName, spriteRect, 10, true),
            Factory::createPlayerCollider(),
            Health_t{200, 200},
            SendUpdate_t{}
        );
    return player;
}
```

#### Créer un ennemi
```cpp
Entity createEnemy(ECS& ecs, float x, float y, const std::string& textureName,
                  MovementPattern_t pattern);
```

#### Créer un projectile
```cpp
Entity createProjectile(ECS& ecs, float x, float y, float vx, float vy,
                       uint8_t team, int damage, const std::string& textureName);
```

#### Créer une étoile (background)
```cpp
Entity createStar(ECS& ecs, float x, float y, float speed, uint8_t brightness);
```

#### Créer une tuile destructible
```cpp
Entity createDestructibleTile(ECS& ecs, float x, float y);
```

#### Créer un bouton de menu
```cpp
Entity createButton(ECS& ecs, float x, float y, const std::string& text,
                   std::function<void()> callback, ButtonType type);
```

### Helpers de collision
```cpp
// Colliders standardisés pour garantir cohérence client/serveur
Collider_t createPlayerCollider();        // 64x64, team 1, 50 damage
Collider_t createEnemyCollider();         // 64x64, team 2, 30 damage
Collider_t createProjectileCollider(uint8_t team, int damage);  // 16x8
Collider_t createTileCollider();          // Basé sur la taille du sprite
```

### Helpers de création de Drawable
```cpp
// Drawable simple (une seule frame)
Drawable_t createDrawable(const std::string& textureName, sf::IntRect rect,
                         int layer, bool visible = true, 
                         float scale = 1.f, float rotation = 0.f);

// Drawable animé (plusieurs frames)
Drawable_t createAnimatedDrawable(const std::string& textureName,
                                 const std::vector<sf::IntRect>& frames,
                                 float animationSpeed, bool loop, int layer,
                                 bool visible = true, float scale = 1.f,
                                 float rotation = 0.f);
```

### Helpers audio
```cpp
PlaySound_t createSound(const std::string& soundId, 
                       float volume = 100.f, float pitch = 1.0f);

BackgroundMusic_t createMusic(const std::string& musicId,
                             bool looping = true, float volume = 50.f);
```

---

## Resource Manager

Gestion centralisée des ressources (textures, fonts, sprites).

```cpp
class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    bool initialize();
    
    // Textures
    void loadTextureFromAtlas(const std::string& atlas);
    sf::Texture& getTexture(const std::string& name);
    sf::IntRect getSpriteRect(const std::string& name);
    
    // Fonts
    bool loadFont(const std::string& id, const std::string& filepath);
    const sf::Font& getFont(const std::string& id) const;
};
```

### Initialisation
```cpp
ResourceManager& rm = ResourceManager::getInstance();
if (!rm.initialize()) {
    std::cerr << "Failed to initialize ResourceManager" << std::endl;
    return -1;
}
```

### Texture Packer
Le ResourceManager utilise un TexturePacker pour optimiser les textures:
```cpp
packer.loadImageFromPath("player", "assets/sprites/entities/ship1.png");
packer.loadImageFromPath("enemy", "assets/sprites/entities/ship2.png");
packer.packTextures();
packer.generateAtlas("assets/sprites/atlas.png");
packer.saveJSON("assets/sprites/atlas.json");
```

---

## Guide d'utilisation

### 1. Initialisation de base

```cpp
#include "ecs.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/PhysicsSystem.hpp"
#include "engine/systems/CollisionSystem.hpp"
#include "engine/systems/RessourceManager.hpp"
#include "engine/EntityFactory.hpp"

int main() {
    // 1. Créer la fenêtre
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Mon Jeu");
    
    // 2. Initialiser le ResourceManager
    ResourceManager& rm = ResourceManager::getInstance();
    rm.initialize();
    rm.loadFont("main", "assets/font/main.ttf");
    
    // 3. Créer l'ECS
    ECS ecs;
    
    // 4. Créer les systèmes
    RenderSystem renderSystem(ecs, window, rm);
    InputSystem inputSystem(ecs);
    PhysicsSystem physicsSystem(ecs, 980.f);
    CollisionSystem collisionSystem(ecs);
    
    // 5. Game loop
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        // Événements
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Update
        inputSystem.update(dt);
        physicsSystem.update(dt);
        collisionSystem.update(dt);
        
        // Render
        window.clear();
        renderSystem.update(dt);
        window.display();
    }
    
    return 0;
}
```

### 2. Créer une entité joueur

```cpp
// Créer le joueur
Entity player = Factory::createPlayer(ecs, 100.f, 540.f, 0, "player");

// Le joueur a automatiquement:
// - Position (100, 540)
// - Velocity
// - PlayerController (id = 0)
// - Drawable (texture "player", layer 10)
// - Collider (64x64, team 1)
// - Health (200 HP)
```

### 3. Créer des ennemis avec patterns

```cpp
// Pattern sinusoïdal
MovementPattern_t sinPattern{
    MovementPattern_t::Type::Sinus,
    100.f,  // amplitude
    2.f,    // frequency
    0.f,    // radius (non utilisé pour sinus)
    200.f   // speed
};

Entity enemy = Factory::createEnemy(ecs, 1800.f, 300.f, "enemy", sinPattern);

// Pattern circulaire
MovementPattern_t circlePattern{
    MovementPattern_t::Type::Circle,
    0.f,    // amplitude (non utilisé)
    0.f,    // frequency (non utilisé)
    150.f,  // radius
    100.f   // speed
};

Entity enemy2 = Factory::createEnemy(ecs, 1600.f, 500.f, "enemy", circlePattern);
```

### 4. Tirer des projectiles

```cpp
// Obtenir la position et contrôleur du joueur
auto* pos = ecs.getComponent<Position_t>(player);
auto* ctrl = ecs.getComponent<PlayerController_t>(player);

if (ctrl->shootCooldown <= 0.f && inputSystem.isActionActive(GameAction::Shoot)) {
    // Créer un projectile
    Entity bullet = Factory::createProjectile(
        ecs,
        pos->x + 50.f,  // Décalage devant le joueur
        pos->y,
        500.f,          // Vitesse X
        0.f,            // Vitesse Y
        1,              // Team joueur
        25,             // Dégâts
        "bullet"
    );
    
    // Reset cooldown
    ctrl->shootCooldown = SHOOT_DELAY;
}

// Décrémenter cooldown
if (ctrl->shootCooldown > 0.f) {
    ctrl->shootCooldown -= dt;
}
```

### 5. Ajouter des effets sonores

```cpp
// Initialiser le SoundSystem
SoundSystem soundSystem(ecs);
soundSystem.loadSound("shoot", "assets/sound/shoot.wav");
soundSystem.loadSound("explosion", "assets/sound/explosion.wav");
soundSystem.loadMusic("bgm", "assets/music/level1.ogg");

// Jouer un son au tir
Entity soundEntity = ecs.createEntity();
ecs.addComponent<PlaySound_t>(soundEntity, 
    Factory::createSound("shoot", 80.f, 1.0f));

// Jouer la musique de fond
Entity musicEntity = ecs.createEntity();
ecs.addComponent<BackgroundMusic_t>(musicEntity,
    Factory::createMusic("bgm", true, 50.f));

// Update le système
soundSystem.update(dt);
```

### 6. Créer des explosions de particules

```cpp
// Initialiser le ParticleSystem
ParticleSystem particleSystem(ecs, 5000);  // Pool de 5000 particules

// Créer une explosion à la mort d'un ennemi
auto entities = ecs.getEntitiesByComponents<Health_t, Position_t>();
for (Entity e : entities) {
    auto* health = ecs.getComponent<Health_t>(e);
    auto* pos = ecs.getComponent<Position_t>(e);
    
    if (health->current <= 0) {
        // Explosion de particules
        particleSystem.emitExplosion(pos->x, pos->y, 50);
        
        // Détruire l'entité
        ecs.killEntity(e);
    }
}

// Render les particules
particleSystem.update(dt);
particleSystem.render(window);
```

### 7. Utiliser le système de physique

```cpp
// Créer une entité avec physique
Entity platformer = ecs.createEntity();
ecs.addComponents<Position_t, Velocity_t, RigidBody_t, Jumper_t, Collider_t>
    (platformer,
        Position_t{100.f, 500.f},
        Velocity_t{0.f, 0.f},
        RigidBody_t{
            1.f,      // mass
            0.5f,     // drag
            1.f,      // gravityScale
            true,     // useGravity
            false     // isKinematic
        },
        Jumper_t{
            500.f,    // jumpForce
            0.1f,     // coyoteTime
            0.1f      // jumpBufferTime
        },
        Collider_t{32.f, 32.f, true, 1, 0}
    );

// Dans l'update, gérer le saut
auto* jumper = ecs.getComponent<Jumper_t>(platformer);
auto* vel = ecs.getComponent<Velocity_t>(platformer);

if (inputSystem.wasActionPressed(GameAction::Shoot) && jumper->isGrounded) {
    vel->y = -jumper->jumpForce;  // Impulsion vers le haut
}
```

### 8. Créer un menu

```cpp
#include "engine/systems/MenuSystem.hpp"
#include "engine/systems/OptionsMenuFactory.hpp"

// Créer un menu principal
MenuSystem menuSystem(ecs);

// Bouton "Jouer"
Entity playButton = Factory::createButton(
    ecs, 960.f, 400.f, "PLAY",
    []() { 
        std::cout << "Starting game..." << std::endl;
        // Code pour démarrer le jeu
    },
    ButtonType::Play
);

// Bouton "Options"
Entity optionsButton = Factory::createButton(
    ecs, 960.f, 500.f, "OPTIONS",
    []() {
        std::cout << "Opening options..." << std::endl;
    },
    ButtonType::Options
);

// Bouton "Quitter"
Entity quitButton = Factory::createButton(
    ecs, 960.f, 600.f, "QUIT",
    [&window]() {
        window.close();
    },
    ButtonType::Quit
);

// Update menu
menuSystem.update(dt);
```

### 9. Charger des vagues d'ennemis

```cpp
WaveSystem waveSystem(ecs);
waveSystem.loadWaveFile("assets/waves/level1.txt");

// Fichier level1.txt:
// delay enemyType movementType count x y
// 2.0 enemy1 1 5 1800 200
// 3.0 enemy2 2 3 1800 500
// 5.0 enemy1 3 10 1800 400

// Le WaveSystem créera automatiquement les ennemis
waveSystem.update(dt);
```

### 10. Gérer plusieurs scènes

```cpp
#include "engine/systems/SceneManager.hpp"

SceneManager sceneManager;

// Ajouter des scènes
sceneManager.addScene("menu");
sceneManager.addScene("game");
sceneManager.addScene("gameover");

// Activer la scène de menu
sceneManager.setActiveScene("menu");

// Dans la game loop
std::string currentScene = sceneManager.getActiveSceneId();
if (currentScene == "menu") {
    menuSystem.update(dt);
} else if (currentScene == "game") {
    physicsSystem.update(dt);
    collisionSystem.update(dt);
    // ... autres systèmes de jeu
}

// Changer de scène
if (gameStarted) {
    sceneManager.setActiveScene("game");
}
```

### 11. Afficher l'HUD (score + ping)

Le client dessine automatiquement un HUD collé en haut à gauche de la vue active:

- **Score** : mis à jour en continu pendant la partie.
- **Ping réseau** : valeur RTT mesurée via un échange ping/pong UDP dédié. La couleur change selon la qualité (vert < 80 ms, orange < 140 ms, rouge au-delà).

Les textes sont recentrés à chaque frame en fonction de la vue courante (caméra en jeu ou vue par défaut dans les menus). Aucune configuration n'est requise : le HUD est créé au démarrage et suit la caméra même quand elle se déplace.

---

## Exemples

### Exemple 1: Shoot'em up basique

```cpp
#include "ecs.hpp"
#include "engine/EntityFactory.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/CollisionSystem.hpp"
#include "engine/systems/MovementSystem.hpp"
#include "engine/systems/SoundSystem.hpp"

class ShootEmUp {
private:
    sf::RenderWindow window;
    ECS ecs;
    ResourceManager& rm;
    
    RenderSystem renderSystem;
    InputSystem inputSystem;
    CollisionSystem collisionSystem;
    MovementSystem movementSystem;
    SoundSystem soundSystem;
    
    Entity player;
    float spawnTimer = 0.f;
    
public:
    ShootEmUp()
        : window(sf::VideoMode(1920, 1080), "R-Type")
        , rm(ResourceManager::getInstance())
        , renderSystem(ecs, window, rm)
        , inputSystem(ecs)
        , collisionSystem(ecs)
        , movementSystem(ecs)
        , soundSystem(ecs)
    {
        rm.initialize();
        soundSystem.loadSound("shoot", "assets/sound/shoot.wav");
        soundSystem.loadSound("explosion", "assets/sound/explosion.wav");
        
        // Créer le joueur
        player = Factory::createPlayer(ecs, 200.f, 540.f, 0, "player");
        
        // Créer le fond étoilé
        for (int i = 0; i < 200; ++i) {
            float x = rand() % 1920;
            float y = rand() % 1080;
            float speed = 50.f + (rand() % 150);
            Factory::createStar(ecs, x, y, speed, 255);
        }
    }
    
    void run() {
        sf::Clock clock;
        
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            
            handleEvents();
            update(dt);
            render();
        }
    }
    
private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    
    void update(float dt) {
        // Mouvement joueur
        auto* pos = ecs.getComponent<Position_t>(player);
        auto* vel = ecs.getComponent<Velocity_t>(player);
        auto* ctrl = ecs.getComponent<PlayerController_t>(player);
        
        vel->x = 0.f;
        vel->y = 0.f;
        
        if (inputSystem.isActionActive(GameAction::MoveUp))
            vel->y = -400.f;
        if (inputSystem.isActionActive(GameAction::MoveDown))
            vel->y = 400.f;
        if (inputSystem.isActionActive(GameAction::MoveLeft))
            vel->x = -400.f;
        if (inputSystem.isActionActive(GameAction::MoveRight))
            vel->x = 400.f;
        
        pos->x += vel->x * dt;
        pos->y += vel->y * dt;
        
        // Tir
        if (ctrl->shootCooldown > 0.f)
            ctrl->shootCooldown -= dt;
        
        if (inputSystem.isActionActive(GameAction::Shoot) && ctrl->shootCooldown <= 0.f) {
            Factory::createProjectile(ecs, pos->x + 50.f, pos->y, 600.f, 0.f, 1, 25, "bullet");
            Entity sound = ecs.createEntity();
            ecs.addComponent<PlaySound_t>(sound, Factory::createSound("shoot", 50.f));
            ctrl->shootCooldown = SHOOT_DELAY;
        }
        
        // Spawn ennemis
        spawnTimer += dt;
        if (spawnTimer >= 2.f) {
            MovementPattern_t pattern{
                MovementPattern_t::Type::Sinus,
                100.f, 2.f, 0.f, 200.f
            };
            Factory::createEnemy(ecs, 1900.f, 200.f + (rand() % 600), "enemy", pattern);
            spawnTimer = 0.f;
        }
        
        // Update systèmes
        inputSystem.update(dt);
        movementSystem.update(dt);
        collisionSystem.update(dt);
        soundSystem.update(dt);
        
        // Nettoyer les entités mortes
        auto deadEntities = ecs.getEntitiesByComponents<Health_t, Position_t>();
        for (Entity e : deadEntities) {
            auto* health = ecs.getComponent<Health_t>(e);
            auto* pos = ecs.getComponent<Position_t>(e);
            if (health->current <= 0) {
                Entity sound = ecs.createEntity();
                ecs.addComponent<PlaySound_t>(sound, Factory::createSound("explosion"));
                ecs.killEntity(e);
            }
        }
    }
    
    void render() {
        window.clear(sf::Color::Black);
        renderSystem.update(0.016f);
        window.display();
    }
};

int main() {
    ShootEmUp game;
    game.run();
    return 0;
}
```

### Exemple 2: Platformer simple

```cpp
class Platformer {
private:
    ECS ecs;
    PhysicsSystem physicsSystem;
    Entity player;
    
public:
    Platformer() : physicsSystem(ecs, 980.f) {
        // Créer le joueur
        player = ecs.createEntity();
        ecs.addComponents<Position_t, Velocity_t, RigidBody_t, Jumper_t, Drawable_t, Collider_t>
            (player,
                Position_t{100.f, 500.f},
                Velocity_t{0.f, 0.f},
                RigidBody_t{1.f, 0.5f, 1.f, true, false},
                Jumper_t{500.f, 0.15f, 0.1f},
                Factory::createDrawable("player", sf::IntRect{0,0,32,32}, 10),
                Collider_t{32.f, 32.f, true, 1, 0}
            );
        
        // Créer des plateformes
        for (int i = 0; i < 10; ++i) {
            Entity platform = ecs.createEntity();
            ecs.addComponents<Position_t, Collider_t, RigidBody_t, Drawable_t>
                (platform,
                    Position_t{i * 200.f, 800.f},
                    Collider_t{200.f, 32.f, true, 0, 0},
                    RigidBody_t{0.f, 0.f, 0.f, false, true},  // Kinematic
                    Factory::createDrawable("platform", sf::IntRect{0,0,200,32}, 5)
                );
        }
    }
    
    void update(float dt) {
        auto* vel = ecs.getComponent<Velocity_t>(player);
        auto* jumper = ecs.getComponent<Jumper_t>(player);
        
        // Mouvement horizontal
        vel->x = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            vel->x = -300.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            vel->x = 300.f;
        
        // Saut
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (jumper->isGrounded || jumper->coyoteCounter > 0.f) {
                vel->y = -jumper->jumpForce;
                jumper->hasDoubleJumped = false;
            } else if (jumper->canDoubleJump && !jumper->hasDoubleJumped) {
                vel->y = -jumper->jumpForce;
                jumper->hasDoubleJumped = true;
            }
        }
        
        physicsSystem.update(dt);
    }
};
```

### Exemple 3: Système de particules avancé

```cpp
void createExplosionEffect(ECS& ecs, ParticleSystem& ps, float x, float y) {
    // Explosion principale
    ps.emitExplosion(x, y, 50);
    
    // Étincelles
    ps.emitSparks(x, y, 30);
    
    // Fumée
    ps.emitSmoke(x, y, 20);
    
    // Débris
    ps.emitDebris(x, y, 15);
    
    // Effet personnalisé: onde de choc
    ps.emitCustom(
        x, y,
        60,                           // count
        200.f,                        // speed
        360.f,                        // spread (cercle complet)
        0.5f,                         // lifetime
        sf::Color(255, 200, 100),    // startColor (orange)
        sf::Color(255, 100, 0, 0),   // endColor (transparent)
        8.f,                          // startSize
        0.f,                          // endSize
        0.f                           // gravity
    );
}

// Utilisation
if (enemyDestroyed) {
    createExplosionEffect(ecs, particleSystem, enemyX, enemyY);
}
```

---

## Conseils de performance

### 1. Réutiliser les entités
```cpp
// ❌ Mauvais: Créer/détruire constamment
for (int i = 0; i < 100; ++i) {
    Entity bullet = Factory::createProjectile(...);
    ecs.killEntity(bullet);
}

// ✅ Bon: Pool d'entités
std::vector<Entity> bulletPool;
void initBulletPool() {
    for (int i = 0; i < 100; ++i) {
        bulletPool.push_back(Factory::createProjectile(...));
        auto* drawable = ecs.getComponent<Drawable_t>(bulletPool.back());
        drawable->visible = false;
    }
}
```

### 2. Batch les requêtes
```cpp
// ❌ Mauvais: Requêtes multiples
for (Entity e : someEntities) {
    auto entities = ecs.getEntitiesByComponents<Position_t>();
    // ...
}

// ✅ Bon: Une seule requête
auto entities = ecs.getEntitiesByComponents<Position_t>();
for (Entity e : entities) {
    // ...
}
```

### 3. Utiliser des layers pour le rendu
```cpp
// Séparer clairement les layers pour faciliter le tri
const int LAYER_BACKGROUND = 0;
const int LAYER_OBSTACLES = 5;
const int LAYER_PLAYER = 10;
const int LAYER_ENEMIES = 15;
const int LAYER_PROJECTILES = 20;
const int LAYER_PARTICLES = 50;
const int LAYER_UI = 100;
```

### 4. Limiter le nombre de particules
```cpp
// Pool de particules adapté au besoin
ParticleSystem particleSystem(ecs, 2000);  // Pas plus que nécessaire
```

---

## Adaptation pour d'autres types de jeux

### Pour un RPG
- Ajouter des composants: `Stats_t`, `Inventory_t`, `Quest_t`
- Créer un `DialogueSystem`
- Utiliser le `SceneManager` pour les zones/niveaux
- Ajouter un `TurnBasedCombatSystem` si au tour par tour

### Pour un jeu de stratégie
- Ajouter: `Selectable_t`, `Unit_t`, `Building_t`
- Créer un `SelectionSystem` et `PathfindingSystem`
- Utiliser la `Camera` pour le zoom/pan
- Ajouter un système de fog of war

### Pour un puzzle game
- Simplifier: Retirer `PhysicsSystem`, `CollisionSystem`
- Ajouter: `Grid_t`, `TileType_t`
- Créer un `GridSystem` et `MatchingSystem`
- Focus sur le `InputSystem` pour les interactions

### Pour un jeu multijoueur
- Ajouter `NetworkID_t` aux entités
- Utiliser `SendUpdate_t` pour la synchronisation
- Implémenter un `NetworkSystem`
- Sérialiser les composants pour l'envoi réseau

---

## Conclusion

Ce moteur de jeu ECS est:
- ✅ **Flexible**: Adapté à de nombreux types de jeux
- ✅ **Performant**: Architecture orientée données
- ✅ **Modulaire**: Ajoutez/retirez des systèmes facilement
- ✅ **Extensible**: Créez vos propres composants et systèmes

Pour commencer votre nouveau jeu:
1. Copiez la structure `include/engine/` et `include/ecs.hpp`
2. Définissez vos composants spécifiques dans `Components.hpp`
3. Créez vos systèmes dans `include/engine/systems/`
4. Utilisez les factories pour simplifier la création d'entités
5. Profitez! 🎮

---

**Licence**: EPITECH PROJECT, 2025-2026  
**Auteurs**: L'équipe R-Type

Pour plus d'informations, consultez le code source ou contactez l'équipe de développement.
