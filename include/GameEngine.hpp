/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameEngine Global Header
*/

#pragma once

// Core Architecture
#include "ecs.hpp"

// Components & Data
#include "engine/systems/Components.hpp"

// Resources & Factories
#include "engine/systems/RessourceManager.hpp"
#include "engine/EntityFactory.hpp"
#include "engine/StageFactory.hpp"

// Systems - Logic
#include "engine/systems/MoveSystem.hpp"      // Basic movement & kinematics
#include "engine/systems/MovementSystem.hpp"  // Pattern-based movement
#include "engine/systems/CollisionSystem.hpp" // AABB Collision
#include "engine/systems/PhysicsSystem.hpp"   // Physics & Gravity
#include "engine/systems/WaveSystem.hpp"      // Wave Management

// Systems - Input & Interaction
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/MenuSystem.hpp"

// Systems - Rendering & Audio
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/CameraSystem.hpp"
#include "engine/systems/ParticleSystem.hpp"
#include "engine/systems/SoundSystem.hpp"

// Systems - Scenes
#include "engine/systems/SceneManager.hpp"
