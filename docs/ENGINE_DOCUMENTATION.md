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

### Audio System

The `SoundSystem` handles SFX and Music.

**Setup:**
```cpp
#include "engine/systems/SoundSystem.hpp"

SoundSystem soundSystem(ecs);
soundSystem.loadSound("boom", "assets/sound/explosion.wav");
soundSystem.loadMusic("theme", "assets/music/bgm.ogg");
```

**Playing Sound (SFX):**
Attach a `PlaySound_t` component. It acts as a trigger and is removed automatically after playing.
```cpp
// Trigger a sound
ecs.addComponent<PlaySound_t>(entity, Factory::createSound("boom"));
```

**Playing Music:**
Attach `BackgroundMusic_t`. The system ensures only one BGM plays.
```cpp
ecs.addComponent<BackgroundMusic_t>(musicEntity, Factory::createMusic("theme", true, 50.f));
```

### Input Management

The `InputSystem` abstracts keyboard/joystick into `GameAction`s.

**Setup:**
```cpp
#include "engine/systems/InputSystem.hpp"

InputSystem inputSystem(ecs);
// Default mappings are loaded automatically (Arrows/WASD to Move, Space to Shoot)
```

**Checking Input:**
In your game loop or internal update logic:
```cpp
if (inputSystem.isActionActive(GameAction::MoveRight)) {
    // Logic to move player
}
```

> **Note:** Don't forget to call `inputSystem.update(dt)` in your game loop so it can poll events!

### Physics & Movement

Simple kinematics are handled by `MoveSystem` using `Position_t` and `Velocity_t`.
Collisions are handled by `CollisionSystem` using `Collider_t`.

**Setup:**
```cpp
#include "engine/systems/MoveSystem.hpp"
#include "engine/systems/CollisionSystem.hpp"

MoveSystem moveSystem(ecs);
CollisionSystem colSystem(ecs);

// Add components to entity
ecs.addComponent<Velocity_t>(entity, {10.f, 0.f}); // Move right
ecs.addComponent<Collider_t>(entity, {32.f, 32.f, true, 0, 10}); // Size w/h, solid, team, damage
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
