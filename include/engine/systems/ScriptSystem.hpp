/*
** EPITECH PROJECT, 2026
** r-type-mirror
** File description:
** ScriptSystem
*/
#pragma once
#include "../EntityFactory.hpp"

class ScriptSystem : public ISystem {
public:
    ScriptSystem(ECS& ecs) : ISystem(ecs) {
        bindToLua();
    };
    void update(double dt) override {
        auto entities = _ecs.getEntitiesByComponents<Script_t>();

        for (Entity e : entities) {
            auto* script = _ecs.getComponent<Script_t>(e);
            if (!script || !script->loaded) {
                loadScript(e, script);  // Lazy load
                continue;
            }

            // Appel onUpdate si existe
            if (script->onUpdate.valid()) {
                auto call = script->onUpdate(script->instance, dt, e);  // Passe instance, dt, entity
                if (!call.valid()) {
                    sol::error err = call;
                    std::cerr << "[Lua Error] onUpdate: " << err.what() << std::endl;
                }
            }
        }
    }

private:
    void bindToLua() {
        // Ouverture des bibliothèques Lua standard
        _lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

        // Binder tous les composants comme usertypes (avec properties pour read/write où approprié)

        // SendUpdate_t
        _lua.new_usertype<SendUpdate_t>("SendUpdate",
            "needsUpdate", sol::property(&SendUpdate_t::needsUpdate, &SendUpdate_t::needsUpdate)
        );

        // Position_t
        _lua.new_usertype<Position_t>("Position",
            "x", sol::property(&Position_t::x, &Position_t::x),
            "y", sol::property(&Position_t::y, &Position_t::y)
        );

        // Velocity_t
        _lua.new_usertype<Velocity_t>("Velocity",
            "x", sol::property(&Velocity_t::x, &Velocity_t::x),
            "y", sol::property(&Velocity_t::y, &Velocity_t::y)
        );

        // Drawable_t
        _lua.new_usertype<Drawable_t>("Drawable",
            "textureId", sol::property([](Drawable_t& d) -> std::string { return std::string(d.textureId); }, [](Drawable_t& d, const std::string& id) { std::strncpy(d.textureId, id.c_str(), sizeof(d.textureId) - 1); }),
            "rect", sol::property(&Drawable_t::rect, &Drawable_t::rect),  // sf::IntRect peut needing binding séparé
            "layer", sol::property(&Drawable_t::layer, &Drawable_t::layer),
            "visible", sol::property(&Drawable_t::visible, &Drawable_t::visible),
            "scale", sol::property(&Drawable_t::scale, &Drawable_t::scale),
            "rotation", sol::property(&Drawable_t::rotation, &Drawable_t::rotation)
        );
        // Binder sf::IntRect pour Drawable.rect (si pas déjà inclus via SFML bindings, mais comme SFML pas bindé, on le fait)
        _lua.new_usertype<sf::IntRect>("IntRect",
            sol::constructors<sf::IntRect(int, int, int, int)>(),
            "left", &sf::IntRect::left,
            "top", &sf::IntRect::top,
            "width", &sf::IntRect::width,
            "height", &sf::IntRect::height
        );

        // PlayerController_t
        _lua.new_usertype<PlayerController_t>("PlayerController",
            "playerId", sol::property(&PlayerController_t::playerId, &PlayerController_t::playerId),
            "isShooting", sol::property(&PlayerController_t::isShooting, &PlayerController_t::isShooting),
            "shootCooldown", sol::property(&PlayerController_t::shootCooldown, &PlayerController_t::shootCooldown)
        );

        // Collider_t
        _lua.new_usertype<Collider_t>("Collider",
            "width", sol::property(&Collider_t::width, &Collider_t::width),
            "height", sol::property(&Collider_t::height, &Collider_t::height),
            "solid", sol::property(&Collider_t::solid, &Collider_t::solid),
            "team", sol::property(&Collider_t::team, &Collider_t::team),
            "damage", sol::property(&Collider_t::damage, &Collider_t::damage)
        );

        // Health_t
        _lua.new_usertype<Health_t>("Health",
            "current", sol::property(&Health_t::current, &Health_t::current),
            "max", sol::readonly(&Health_t::max)  // Exemple read-only pour max
        );

        // WaveSpawner_t
        _lua.new_usertype<WaveSpawner_t>("WaveSpawner",
            "nextSpawnTime", sol::property(&WaveSpawner_t::nextSpawnTime, &WaveSpawner_t::nextSpawnTime),
            "currentWave", sol::property(&WaveSpawner_t::currentWave, &WaveSpawner_t::currentWave)
        );

        // WaveData_t
        _lua.new_usertype<WaveData_t>("WaveData",
            "delay", sol::property(&WaveData_t::delay, &WaveData_t::delay),
            "enemyType", sol::property([](WaveData_t& w) -> std::string { return std::string(w.enemyType); }, [](WaveData_t& w, const std::string& type) { std::strncpy(w.enemyType, type.c_str(), sizeof(w.enemyType) - 1); }),
            "count", sol::property(&WaveData_t::count, &WaveData_t::count),
            "x", sol::property(&WaveData_t::x, &WaveData_t::x),
            "y", sol::property(&WaveData_t::y, &WaveData_t::y)
        );

        // MovementPattern_t
        _lua.new_usertype<MovementPattern_t>("MovementPattern",
            "type", sol::property(&MovementPattern_t::type, &MovementPattern_t::type),
            "amplitude", sol::property(&MovementPattern_t::amplitude, &MovementPattern_t::amplitude),
            "frequency", sol::property(&MovementPattern_t::frequency, &MovementPattern_t::frequency),
            "radius", sol::property(&MovementPattern_t::radius, &MovementPattern_t::radius),
            "speed", sol::property(&MovementPattern_t::speed, &MovementPattern_t::speed)
        );
        // Binder l'enum Type
        auto movementEnum = _lua.create_table();
        movementEnum["Linear"] = MovementPattern_t::Type::Linear;
        movementEnum["Sinus"] = MovementPattern_t::Type::Sinus;
        movementEnum["Cosinus"] = MovementPattern_t::Type::Cosinus;
        movementEnum["Circle"] = MovementPattern_t::Type::Circle;
        movementEnum["Zigzag"] = MovementPattern_t::Type::Zigzag;
        movementEnum["Spiral"] = MovementPattern_t::Type::Spiral;
        _lua["MovementType"] = movementEnum;

        // Projectile_t
        _lua.new_usertype<Projectile_t>("Projectile",
            "speed", sol::property(&Projectile_t::speed, &Projectile_t::speed),
            "damage", sol::property(&Projectile_t::damage, &Projectile_t::damage),
            "ownerId", sol::property(&Projectile_t::ownerId, &Projectile_t::ownerId)
        );

        // Obstacle_t
        _lua.new_usertype<Obstacle_t>("Obstacle",
            "blocking", sol::property(&Obstacle_t::blocking, &Obstacle_t::blocking)
        );

        // Star_t
        _lua.new_usertype<Star_t>("Star",
            "speed", sol::property(&Star_t::speed, &Star_t::speed),
            "brightness", sol::property(&Star_t::brightness, &Star_t::brightness),
            "size", sol::property(&Star_t::size, &Star_t::size)
        );

        // JustShot_t
        _lua.new_usertype<JustShot_t>("JustShot",
            "active", sol::property(&JustShot_t::active, &JustShot_t::active)
        );

        // Script_t (optionnel, si Lua veut introspecter ses propres scripts)
        _lua.new_usertype<Script_t>("Script",
            "path", sol::property([](Script_t& s) -> std::string { return std::string(s.path); }, [](Script_t& s, const std::string& p) { std::strncpy(s.path, p.c_str(), sizeof(s.path) - 1); }),
            // instance, onInit, etc. sont sol:: types, pas besoin de binder si pas utilisés en Lua
            "loaded", sol::property(&Script_t::loaded, &Script_t::loaded)
        );

        // Expose ECS wrappers pour getComponent (un par type pour simplicité)
        _lua["getSendUpdate"] = [this](Entity e) -> SendUpdate_t* { return _ecs.getComponent<SendUpdate_t>(e); };
        _lua["getPosition"] = [this](Entity e) -> Position_t* { return _ecs.getComponent<Position_t>(e); };
        _lua["getVelocity"] = [this](Entity e) -> Velocity_t* { return _ecs.getComponent<Velocity_t>(e); };
        _lua["getDrawable"] = [this](Entity e) -> Drawable_t* { return _ecs.getComponent<Drawable_t>(e); };
        _lua["getPlayerController"] = [this](Entity e) -> PlayerController_t* { return _ecs.getComponent<PlayerController_t>(e); };
        _lua["getCollider"] = [this](Entity e) -> Collider_t* { return _ecs.getComponent<Collider_t>(e); };
        _lua["getHealth"] = [this](Entity e) -> Health_t* { return _ecs.getComponent<Health_t>(e); };
        _lua["getWaveSpawner"] = [this](Entity e) -> WaveSpawner_t* { return _ecs.getComponent<WaveSpawner_t>(e); };
        _lua["getMovementPattern"] = [this](Entity e) -> MovementPattern_t* { return _ecs.getComponent<MovementPattern_t>(e); };
        _lua["getProjectile"] = [this](Entity e) -> Projectile_t* { return _ecs.getComponent<Projectile_t>(e); };
        _lua["getObstacle"] = [this](Entity e) -> Obstacle_t* { return _ecs.getComponent<Obstacle_t>(e); };
        _lua["getStar"] = [this](Entity e) -> Star_t* { return _ecs.getComponent<Star_t>(e); };
        _lua["getJustShot"] = [this](Entity e) -> JustShot_t* { return _ecs.getComponent<JustShot_t>(e); };
        _lua["getScript"] = [this](Entity e) -> Script_t* { return _ecs.getComponent<Script_t>(e); };

        // Exemple custom : Factories depuis Lua
        _lua["createProjectile"] = [](ECS& ecs, float x, float y, float velocityX, float velocityY, double teamD, double damageD, const std::string& textureId, double ownerIdD = -1.0) {
            uint8_t team = static_cast<uint8_t>(teamD);  // Convert double to uint8_t
            int damage = static_cast<int>(damageD);
            int ownerId = static_cast<int>(ownerIdD);
            return Factory::createProjectile(ecs, x, y, velocityX, velocityY, team, damage, textureId, ownerId);
        };
        _lua["createEnemy"] = [](ECS& ecs, float x, float y, int health, int patternType, const std::string& textureId) {
            return Factory::createEnemy(ecs, x, y, health, static_cast<MovementPattern_t::Type>(patternType), textureId);
        };
        // Ajoute d'autres factories si besoin (createPlayer, createObstacle, etc.)

        // Binder l'ECS elle-même (limité : e.g., createEntity, killEntity)
        _lua.new_usertype<ECS>("ECS",
            "createEntity", &ECS::createEntity,
            "killEntity", &ECS::killEntity,
            "hasComponent", [](ECS& ecs, Entity e, const std::string& compName) {
                // Exemple basique : Mapper string à ComponentID (ajoute une map si besoin)
                if (compName == "Position") return ecs.hasComponent<Position_t>(e);
                // Ajoute pour chaque...
                return false;
            }
        );
        _lua["ECS"] = std::ref(_ecs);  // Référence à l'ECS (attention à la lifetime)
    }

    void loadScript(Entity e, Script_t* script) {
        if (script->loaded) return;

        auto result = _lua.safe_script_file(script->path, &sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "[Lua Error] Chargement " << script->path << ": " << err.what() << std::endl;
            return;
        }

        // Crée une instance per-entité (table vide, hérite du script global)
        script->instance = _lua.create_table();

        // Extrait callbacks (assume le script définit ces functions)
        script->onInit = _lua["onInit"];  // Global ou per-script
        script->onUpdate = _lua["onUpdate"];
        script->onCollision = _lua["onCollision"];

        // Appel onInit si existe
        if (script->onInit.valid()) {
            auto call = script->onInit(script->instance, e);  // Passe instance et entity
            if (!call.valid()) {
                sol::error err = call;
                std::cerr << "[Lua Error] onInit: " << err.what() << std::endl;
            }
        }

        script->loaded = true;
    }

    sol::state _lua;  // État Lua global (pour bindings)
};