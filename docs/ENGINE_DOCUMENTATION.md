# Game Engine Documentation

## Table of Contents
1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [Architecture](#architecture)
4. [Features & Modules](#features--modules)
   - [Graphics & Rendering](#graphics--rendering)
   - [Audio System](#audio-system)
   - [Input Management](#input-management)
   - [Physics & Movement](#physics--movement)
5. [Complete Example](#complete-example)

---

## Introduction

Welcome to the **R-Type Game Engine** documentation. This engine is built on a high-performance **Entity Component System (ECS)** architecture using C++ and SFML. It provides a modular and data-oriented approach to game development, allowing for flexible system composition (Rendering, Audio, Physics, etc.).

**Key Principles:**
- **Entity**: A unique ID.
- **Component**: Pure data attached to an entity (e.g., Position, Sprite).
- **System**: Logic that processes entities with specific components.

---

## Quick Start

### Prerequisites
- C++20 Compliant Compiler
- CMake
- SFML 2.5+

### Installation
Ensure your project links against the engine library. If you are using CMake, include the headers and link the compiled `r-type_client` or engine library.

You can simply include the global header to access all engine features:
```cpp
#include "GameEngine.hpp"
```

### Basic Application Setup

Here is the minimal boilerplate to get a window running with the engine.

```cpp
#include "GameEngine.hpp"

int main() {
    // 1. Create Window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "My Game");

    // 2. Initialize ECS
    ECS ecs;

    // 3. Initialize Resources
    ResourceManager& rm = ResourceManager::getInstance();
    rm.initialize(); // Loads default assets pack

    // 4. Create Systems
    RenderSystem renderSystem(ecs, window, rm);

    // 5. Game Loop
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        renderSystem.update(dt);
    }
    return 0;
}
```

---

## Architecture

### ECS (Entity Component System)
The core `ECS` class manages everything.

- **Create Entity**: `Entity e = ecs.createEntity();`
- **Add Component**: `ecs.addComponent<Type>(entity, data);`
- **Get Component**: `Type* ptr = ecs.getComponent<Type>(entity);`
- **Kill Entity**: `ecs.killEntity(entity);`

### Systems Lifecycle & Customization

The engine provides base systems, but **you act as the orchestrator**:

1.  **Instantiation**: You must manually instantiate every system you want to use.
2.  **Execution**: You must call `.update(dt)` on each system every frame in your game loop.
3.  **Custom Logic**: For specific game features (e.g., Score, Wave Management, AI), you should **create your own Systems**.

**Creating a Custom System:**
Inherit from `ISystem`, implements `update(dt)`, and use `_ecs` to query entities.

```cpp
class MyGameLogicSystem : public ISystem {
public:
    MyGameLogicSystem(ECS& ecs) : ISystem(ecs) {}

    void update(double dt) override {
        // Example: Find all enemies and make them think
        auto enemies = _ecs.getEntitiesByComponents<EnemyAI_t, Position_t>();
        for (Entity e : enemies) {
            // Your custom logic here
        }
    }
};
```

---

## Features & Modules

### Graphics & Rendering

The `RenderSystem` handles drawing entities. To display something, an entity needs:
1. `Position_t` (Coordinates)
2. `Drawable_t` (Sprite info)

**Setup:**
```cpp
#include "engine/systems/RenderSystem.hpp"

RenderSystem renderSystem(ecs, window, resourceManager);
```

**Adding a Sprite:**
```cpp
#include "engine/EntityFactory.hpp"

// Create entity
Entity ship = ecs.createEntity();

// Add position
ecs.addComponent<Position_t>(ship, {100.f, 200.f});

// Add sprite (using Factory helper)
// "ship1" must be a loaded texture name
ecs.addComponent<Drawable_t>(ship, Factory::createDrawable("ship1", {0, 0, 32, 32}, 1));
```

**Animated Sprites:**
Use `createAnimatedDrawable` to define animation frames.
```cpp
std::vector<sf::IntRect> frames = { {0,0,32,32}, {32,0,32,32} };
ecs.addComponent<Drawable_t>(player, Factory::createAnimatedDrawable("ship1", frames, 0.1f, true, 1));
```

**Layer System:**
The `layer` parameter in `Drawable_t` controls rendering order:
- `0` = Background elements
- `10` = Player and main gameplay objects
- `20` = Bullets and projectiles
- `50` = Particles and effects

**Update Loop:**
```cpp
renderSystem.clear();      // Clear the screen
renderSystem.update(dt);   // Draw all entities with Drawable_t components
```

### Audio System

The `SoundSystem` handles SFX and Music playback.

**Setup:**
```cpp
#include "engine/systems/SoundSystem.hpp"

SoundSystem soundSystem(ecs);

// Load sounds (short effects - stored in memory)
soundSystem.loadSound("boom", "assets/sound/explosion.wav");
soundSystem.loadSound("shoot", "assets/sound/laser.wav");

// Load music (long tracks - streamed from disk)
soundSystem.loadMusic("theme", "assets/music/bgm.ogg");
```

**Playing Sound (SFX):**
Attach a `PlaySound_t` component. It acts as a trigger and is removed automatically after playing.
```cpp
// Trigger a sound
ecs.addComponent<PlaySound_t>(entity, Factory::createSound("boom", 100.f, 1.0f));
// Parameters: soundId, volume (0-100), pitch (0.5-2.0)
```

**Playing Music:**
Attach `BackgroundMusic_t`. The system ensures only one BGM plays at a time.
```cpp
Entity musicEntity = ecs.createEntity();
ecs.addComponent<BackgroundMusic_t>(musicEntity, Factory::createMusic("theme", true, 50.f));
// Parameters: musicId, looping, volume
```

**Volume Control:**
```cpp
soundSystem.setMusicVolume(75.f);  // Set music volume (0-100)
soundSystem.setSoundVolume(80.f);  // Set SFX volume (0-100)
```

**Update Loop:**
```cpp
soundSystem.update(dt);  // Process sound triggers and cleanup
```

### Input Management

The `InputSystem` abstracts keyboard/joystick into `GameAction`s for platform-independent input handling.

**Setup:**
```cpp
#include "engine/systems/InputSystem.hpp"

InputSystem inputSystem(ecs);
// Default mappings are loaded automatically:
// - Arrows/WASD → Move actions
// - Space → Shoot
// - Enter → MenuSelect
// - Escape → Quit/Pause
```

**Checking Input:**
In your game loop or system update:
```cpp
if (inputSystem.isActionActive(GameAction::MoveRight)) {
    // Move player right
}
if (inputSystem.isActionActive(GameAction::Shoot)) {
    // Fire weapon
}
```

**Available GameActions:**
- `MoveUp`, `MoveDown`, `MoveLeft`, `MoveRight`
- `Shoot`
- `Quit`, `Pause`
- `MenuUp`, `MenuDown`, `MenuSelect`
- `ToggleDebug`

**Update Loop:**
```cpp
inputSystem.update(dt);  // Poll events and update action states
```

> **Note:** Always call `inputSystem.update(dt)` in your game loop before checking inputs!

### Physics & Movement

#### MoveSystem
Simple kinematics handled by `MoveSystem` using `Position_t` and `Velocity_t`.

**Setup:**
```cpp
#include "engine/systems/MoveSystem.hpp"

MoveSystem moveSystem(ecs);
```

**Adding Movement:**
```cpp
// Create a moving entity
Entity bullet = ecs.createEntity();
ecs.addComponent<Position_t>(bullet, {100.f, 200.f});
ecs.addComponent<Velocity_t>(bullet, {300.f, 0.f});  // 300 pixels/sec to the right
```

**Update Loop:**
```cpp
moveSystem.update(dt);  // Applies velocity to position: pos += vel * dt
```

#### CollisionSystem
Handles AABB collision detection and damage application.

**Setup:**
```cpp
#include "engine/systems/CollisionSystem.hpp"

CollisionSystem colSystem(ecs);
```

**Adding Colliders:**
```cpp
// Player collider
ecs.addComponent<Collider_t>(player, Factory::createPlayerCollider());
// Defaults: {64x64, solid, team=1, damage=50}

// Enemy collider
ecs.addComponent<Collider_t>(enemy, Factory::createEnemyCollider());
// Defaults: {64x64, solid, team=2, damage=30}

// Projectile collider
ecs.addComponent<Collider_t>(bullet, Factory::createProjectileCollider(1, 25));
// Parameters: team, damage
```

**Health Component:**
Add `Health_t` to make entities damageable:
```cpp
ecs.addComponent<Health_t>(enemy, {100, 100, -1, 0.f});
// Parameters: max HP, current HP, lastAttackerId, contactCooldown
```

**Team System:**
- `team = 0`: Neutral (doesn't collide with others)
- `team = 1`: Player team
- `team = 2`: Enemy team
- Different teams damage each other on collision

**Update Loop:**
```cpp
colSystem.update(dt);  // Detect collisions and apply damage
```

### Camera System

The `CameraSystem` provides a 2D camera with smooth following, zoom, shake effects, and world bounds.

**Setup:**
```cpp
#include "engine/systems/CameraSystem.hpp"

CameraSystem cameraSystem(ecs, window);
```

**Creating a Camera:**
```cpp
Entity camera = ecs.createEntity();
Camera_t cam;
cam.x = 960.f;           // Initial position
cam.y = 540.f;
cam.smoothSpeed = 5.f;   // Follow speed (higher = more responsive)
cam.zoom = 1.f;
cam.viewWidth = 1920.f;
cam.viewHeight = 1080.f;
ecs.addComponent<Camera_t>(camera, cam);
```

**Camera Follow Target:**
Mark an entity for the camera to follow:
```cpp
ecs.addComponent<CameraTarget_t>(player, {0.f, 0.f});
// Parameters: offsetX, offsetY (offset from entity position)
```

**Camera Effects:**
```cpp
// Screen shake (e.g., on explosion)
cameraSystem.shake(10.f, 0.5f);  // intensity, duration

// Smooth zoom
cameraSystem.setTargetZoom(2.0f);  // 2x zoom, smooth transition

// Instant zoom
cameraSystem.setZoom(1.5f);

// Set world bounds (prevent camera from showing outside game area)
cameraSystem.setBounds(0.f, 0.f, 3840.f, 1080.f);  // minX, minY, maxX, maxY
```

**Update Loop:**
```cpp
cameraSystem.update(dt);      // Update camera position, zoom, shake
cameraSystem.applyView();     // Apply view to window before rendering
```

### Particle System

The `ParticleSystem` provides a high-performance particle effect system with object pooling.

**Setup:**
```cpp
#include "engine/systems/ParticleSystem.hpp"

ParticleSystem particleSystem(ecs, 2000);  // Pool size: 2000 particles
```

**Preset Effects:**
```cpp
// Explosion (radial burst)
particleSystem.emitExplosion(x, y, 30);  // position, particle count

// Sparks (small, fast with gravity)
particleSystem.emitSparks(x, y, 15);

// Smoke (rises and fades)
particleSystem.emitSmoke(x, y, 10);

// Debris (large chunks that fall)
particleSystem.emitDebris(x, y, 8);

// Trail (follows behind moving object)
particleSystem.emitTrail(x, y, directionDegrees, 3);
```

**Custom Particles:**
```cpp
particleSystem.emitCustom(
    x, y,                                    // Position
    50,                                      // Particle count
    200.f,                                   // Speed
    360.f,                                   // Spread angle (360 = full circle)
    1.5f,                                    // Lifetime in seconds
    sf::Color::Yellow,                       // Start color
    sf::Color(255, 100, 0, 0),              // End color (transparent orange)
    8.f,                                     // Start size
    0.f,                                     // End size (shrinks to nothing)
    100.f                                    // Gravity (positive = downward)
);
```

**Particle Emitters:**
Attach an emitter to an entity for continuous effects:
```cpp
Entity thruster = ecs.createEntity();
ecs.addComponent<Position_t>(thruster, {playerX, playerY});

ParticleEmitter_t emitter;
emitter.type = ParticleType::Trail;
emitter.spawnRate = 50.f;          // Particles per second
emitter.particleLifetime = 0.5f;
emitter.speed = 100.f;
emitter.direction = 180.f;          // Shoot backward
emitter.spread = 20.f;              // Slight cone
emitter.startColor = sf::Color::Cyan;
emitter.endColor = sf::Color(0, 100, 200, 0);
emitter.active = true;
emitter.burst = false;              // Continuous emission

ecs.addComponent<ParticleEmitter_t>(thruster, emitter);
```

**Update Loop:**
```cpp
particleSystem.update(dt);           // Update particle physics
particleSystem.render(window);       // Render particles (call after renderSystem)
```

### Menu System

The `MenuSystem` handles interactive UI menus with keyboard, mouse, and gamepad support.

**Setup:**
```cpp
#include "engine/systems/MenuSystem.hpp"
#include "engine/Menu.hpp"

MenuSystem menuSystem(ecs, inputSystem);
menuSystem.setWindow(&window);
menuSystem.setSoundSystem(&soundSystem);
```

**Creating Menu Items:**
```cpp
// Title text
Entity title = ecs.createEntity();
ecs.addComponent<Position_t>(title, {960.f, 200.f});
Text_t titleText;
std::strcpy(titleText.text, "MAIN MENU");
titleText.fontSize = 72;
titleText.color = sf::Color::White;
titleText.centered = true;
ecs.addComponent<Text_t>(title, titleText);

// Button
Entity playButton = ecs.createEntity();
ecs.addComponent<Position_t>(playButton, {960.f, 400.f});
Text_t buttonText;
std::strcpy(buttonText.text, "Play Game");
buttonText.fontSize = 48;
buttonText.color = sf::Color::White;
buttonText.centered = true;
ecs.addComponent<Text_t>(playButton, buttonText);

MenuItem_t menuItem;
menuItem.action = MenuAction::StartGame;
menuItem.isSelectable = true;
ecs.addComponent<MenuItem_t>(playButton, menuItem);

// Highlight effect
Highlight_t highlight;
highlight.selectedColor = sf::Color::Yellow;
highlight.selectedScale = 1.2f;
ecs.addComponent<Highlight_t>(playButton, highlight);
```

**Menu Actions:**
Register callbacks for menu actions:
```cpp
menuSystem.registerAction("start_game", []() {
    // Load game level
});

menuSystem.registerAction("quit", []() {
    window.close();
});
```

**Volume Sliders:**
```cpp
Entity volumeSlider = ecs.createEntity();
ecs.addComponent<Position_t>(volumeSlider, {960.f, 500.f});

Slider_t slider;
slider.minValue = 0.f;
slider.maxValue = 100.f;
slider.currentValue = 50.f;
slider.step = 5.f;
slider.linkedSetting = "music_volume";
ecs.addComponent<Slider_t>(volumeSlider, slider);
ecs.addComponent<MenuItem_t>(volumeSlider, {MenuAction::None, false, true});
```

**Update Loop:**
```cpp
// Handle SFML events
sf::Event event;
while (window.pollEvent(event)) {
    menuSystem.handleMouseInput(event);
}

menuSystem.update(dt);  // Process menu navigation and actions
```

### Scene Management

The `SceneManager` handles switching between different game scenes (menus, levels, etc.).

**Setup:**
```cpp
#include "engine/systems/SceneManager.hpp"

auto sceneManager = std::make_unique<SceneManager>();
sceneManager->addScene("main_menu");
sceneManager->addScene("game");
sceneManager->addScene("settings");
sceneManager->addScene("pause");

menuSystem.setSceneManager(std::move(sceneManager));
```

**Switching Scenes:**
```cpp
sceneManager->setActiveScene("game");  // Load game scene
```

**Checking Active Scene:**
```cpp
if (sceneManager->getActiveSceneId() == "game") {
    // Run game logic
} else if (sceneManager->getActiveSceneId() == "main_menu") {
    // Show menu
}
```

### Advanced Physics System

For platformer-style physics with gravity, jumping, and platforms, use the advanced physics components.

**RigidBody Component:**
```cpp
Entity platformer = ecs.createEntity();
ecs.addComponent<Position_t>(platformer, {100.f, 100.f});
ecs.addComponent<Velocity_t>(platformer, {0.f, 0.f});

RigidBody_t rb;
rb.mass = 1.f;
rb.gravityScale = 1.f;       // Normal gravity
rb.drag = 0.1f;              // Air resistance
rb.bounciness = 0.0f;        // No bounce
rb.useGravity = true;
rb.isKinematic = false;
ecs.addComponent<RigidBody_t>(platformer, rb);
```

**Jumping:**
```cpp
Jumper_t jumper;
jumper.jumpForce = 500.f;
jumper.maxJumps = 2;          // Double jump
jumper.coyoteTime = 0.1f;     // Grace period after leaving platform
jumper.jumpBufferTime = 0.1f; // Early jump input buffer
ecs.addComponent<Jumper_t>(platformer, jumper);
```

**Platforms:**
```cpp
Entity platform = ecs.createEntity();
ecs.addComponent<Position_t>(platform, {400.f, 500.f});

Platform_t plat;
plat.oneWay = false;          // Solid from all sides
plat.friction = 1.f;          // Normal friction
plat.velocity = {50.f, 0.f};  // Moving platform (optional)
ecs.addComponent<Platform_t>(platform, plat);

BoxCollider_t collider;
collider.width = 200.f;
collider.height = 32.f;
ecs.addComponent<BoxCollider_t>(platform, collider);
```

**Trigger Zones:**
```cpp
Entity trigger = ecs.createEntity();
ecs.addComponent<Position_t>(trigger, {800.f, 400.f});

BoxCollider_t triggerCollider;
triggerCollider.width = 100.f;
triggerCollider.height = 100.f;
triggerCollider.isTrigger = true;  // Doesn't block movement
ecs.addComponent<BoxCollider_t>(trigger, triggerCollider);

TriggerZone_t zone;
zone.type = TriggerZone_t::Type::Teleporter;
zone.targetScene = "level_2";
zone.teleportPos = {100.f, 100.f};
zone.onTrigger = [](Entity e) {
    // Custom callback when triggered
};
ecs.addComponent<TriggerZone_t>(trigger, zone);
```

---

## Complete Example

This example demonstrates a player ship moving and shooting.

```cpp
#include "GameEngine.hpp"

int main() {
    // 1. Setup Window & Engine
    sf::RenderWindow window(sf::VideoMode(1200, 800), "R-Type Example");
    window.setFramerateLimit(60);
    
    ECS ecs;
    ResourceManager& rm = ResourceManager::getInstance();
    rm.initialize(); // Load textures

    // 2. Setup Systems
    RenderSystem renderSystem(ecs, window, rm);
    InputSystem inputSystem(ecs);
    MoveSystem movementSystem(ecs);
    SoundSystem soundSystem(ecs);

    // 3. Load Audio
    soundSystem.loadSound("shoot", "assets/sound/shoot.wav");

    // 4. Create Player
    Entity player = ecs.createEntity();
    ecs.addComponent<Position_t>(player, {100.f, 300.f});
    ecs.addComponent<Velocity_t>(player, {0.f, 0.f});
    ecs.addComponent<Drawable_t>(player, 
        Factory::createDrawable("ship1", rm.getSpriteRect("ship1"), 2));
    ecs.addComponent<PlayerController_t>(player, {0, false, 0.f});

    sf::Clock clock;

    // 5. Game Loop
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        // Handle SFML Events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // --- Game Logic ---
        
        // Input Handling
        auto* pos = ecs.getComponent<Position_t>(player);
        auto* vel = ecs.getComponent<Velocity_t>(player);

        vel->x = 0; vel->y = 0;
        if (inputSystem.isActionActive(GameAction::MoveUp))    vel->y = -300.f;
        if (inputSystem.isActionActive(GameAction::MoveDown))  vel->y =  300.f;
        if (inputSystem.isActionActive(GameAction::MoveLeft))  vel->x = -300.f;
        if (inputSystem.isActionActive(GameAction::MoveRight)) vel->x =  300.f;

        // Shooting
        if (inputSystem.isActionActive(GameAction::Shoot)) {
            // Create bullet logic here
            // ecs.addComponent<PlaySound_t>(player, Factory::createSound("shoot"));
        }

        // --- System Updates ---
        inputSystem.update(dt);    // Process input
        movementSystem.update(dt); // Apply Velocity to Position
        soundSystem.update(dt);    // Process sounds
        
        // Rendering
        renderSystem.clear();      // Clear the screen
        renderSystem.update(dt);   // Draw everything
    }

    return 0;
}
```
