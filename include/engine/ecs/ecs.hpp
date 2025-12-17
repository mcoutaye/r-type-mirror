/*
** EPITECH PROJECT, 2025
** G-CPP-500-RUN-5-2-rtype-3
** File description:
** ecs
*/
#ifndef ECS_HPP
#define ECS_HPP

#include <queue>
#include <cstdint>
#include <cassert>
#include <vector>
#include <limits>
#include <bitset>
#include <unordered_map>

/**
 * @brief Maximum number of entities allowed in the ECS system.
 * 
 * This constant defines the upper limit for entities, set to 2048 which is sufficient for the project.
 */
static constexpr size_t MAX_ENTITIES = 1 << 11; // 2048 Entities Max (Enough for our project)

/**
 * @brief Maximum number of components per entity.
 * 
 * This constant limits the number of component types to 128.
 */
static constexpr size_t MAX_COMPONENT = 1 << 7; // 128 Components max par Entites

/**
 * @brief Type alias for Entity, which is simply an ID.
 */
using Entity = std::uint32_t; // Entity is just an ID

/**
 * @brief Type alias for Signature, a bitset to check if an entity has certain components.
 */
using Signature = std::bitset<MAX_COMPONENT>; // The checker for an entity has a certain component

/**
 * @brief Type alias for ComponentTypeID, used to identify component types.
 */
using ComponentTypeID = std::size_t; // TypeID's for ComposentType

// ### COMPONENT TYPE ID ###

/**
 * @brief Generates a new unique ComponentTypeID.
 * 
 * This function increments and returns a static counter to provide unique IDs for component types.
 * 
 * @return A new ComponentTypeID.
 */
inline ComponentTypeID newComponentTypeID()
{
    static ComponentTypeID last = 0;
    return last++;
}

/**
 * @brief Retrieves the unique ID for a given component type.
 * 
 * This template function ensures each component type has a unique static ID.
 * 
 * @tparam T The component type.
 * @return The ComponentTypeID for type T.
 */
template <typename T>
ComponentTypeID getComponentID()
{
    static ComponentTypeID id = newComponentTypeID();
    return id;
}

// ### STORAGE COMPONENTS ###

/**
 * @class StorageComponent
 * @brief Manages storage for a specific type of component.
 * 
 * This class stores components of the same type for all entities and uses a bitset to track which entities have this component.
 * 
 * @tparam ComponentType The type of component being stored.
 */
template<typename ComponentType>
class StorageComponent
{
public:
    /**
     * @brief Constructor for StorageComponent.
     * 
     * Initializes the storage vector and resets the checker bitset.
     */
    StorageComponent()
    {
        __Storage.resize(MAX_ENTITIES); // Posibly one for every entity
        __checker.reset();
    }

    /**
     * @brief Destructor for StorageComponent.
     */
    ~StorageComponent() = default;

    // FUCK storage : Find, Update, Create & Kill for component

    /**
     * @brief Adds a component to the specified entity.
     * 
     * @param e The entity to add the component to.
     * @param component The component to add.
     */
    void add(Entity e, const ComponentType &component)
    {
        assert(e < MAX_ENTITIES);
        __Storage[e] = component; //component for 'e' entity is in [e]
        __checker.set(e); // Update checker
    }

    /**
     * @brief Checks if the entity has this component type.
     * 
     * @param e The entity to check.
     * @return True if the entity has the component, false otherwise.
     */
    bool has(Entity e) const
    {
        assert(e < MAX_ENTITIES);
        return __checker.test(e); // Check 'e' bit to see if 'e' entity has this component type
    }

    /**
     * @brief Retrieves a read-only pointer to the component for the entity.
     * 
     * @param e The entity.
     * @return Const pointer to the component, or nullptr if not present.
     */
    const ComponentType *get(Entity e) const // READ-ONLY version because const (FIND)
    {
        assert(e < MAX_ENTITIES);
        if (!has(e))
            return nullptr;
        return &__Storage[e]; // Reference to the component to modify it IN SYSTEM ONLY
    }

    /**
     * @brief Retrieves a mutable pointer to the component for the entity.
     * 
     * @param e The entity.
     * @return Pointer to the component, or nullptr if not present.
     */
    ComponentType *get(Entity e) // get version for UPDATE
    {
        assert(e < MAX_ENTITIES);
        if (!has(e))
            return nullptr;
        return &__Storage[e];
    }

    /**
     * @brief Removes the component from the entity.
     * 
     * @param e The entity to remove the component from.
     */
    void kill(Entity e)
    {
        assert(e < MAX_ENTITIES);
        __checker.reset(e);
        // No need to remove in __Storage, the [e] space is for this entity anyways.
        // In case of removing/re-adding, its better since we just change one bit instead of playing with memory
    }

private:
    // Where the components are stocked.
    std::vector<ComponentType> __Storage;
    // Those bits are check to know if an entity has a component of this specific type
    // It doesnt tell which one, just if it has this type or not.
    std::bitset<MAX_ENTITIES> __checker;
};

/**
 * @class ECS
 * @brief Entity Component System manager.
 * 
 * This class handles entity creation, component management, and signatures for efficient querying.
 */
class ECS
{
    public:
        /**
         * @brief Constructor for ECS.
         * 
         * Initializes free entities queue and asserts it's not empty.
         */
        ECS()
        {
            for (Entity e = 0; e < MAX_ENTITIES; e++)
                __freeEntities.push(e);
            assert(!__freeEntities.empty());
        }

        /**
         * @brief Destructor for ECS.
         */
        ~ECS() = default;

        // ## 'FUCK' all entities ##

        /**
         * @brief Creates a new entity.
         * 
         * @return The ID of the newly created entity.
         */
        Entity createEntity()
        {
            assert(__livingEntities < MAX_ENTITIES);
            Entity id = __freeEntities.front();
            __freeEntities.pop();
            __livingEntities++;
            return id;
        }

        /**
         * @brief Destroys an entity.
         * 
         * @param e The entity to destroy.
         */
        void killEntity(Entity e)
        {
            assert(e < MAX_ENTITIES);
            __signatures[e].reset();
            __freeEntities.push(e);
            __livingEntities--;
        }

        /**
         * @brief Retrieves the signature of an entity.
         * 
         * @param e The entity.
         * @return The signature bitset.
         */
        Signature getSignature(Entity e) const
        {
            assert(e < MAX_ENTITIES);
            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return Signature(); // An empty bitset
            return it->second; // The Entity is first in the unordered map, their signature is second
        }

        /**
         * @brief Sets the signature for an entity.
         * 
         * @param e The entity.
         * @param sig The new signature.
         */
        void setSignature(Entity e, Signature sig)
        {
            assert(e < MAX_ENTITIES);
            __signatures[e] = sig;
        }

        // ### Composant manager ###

        // 'FUCK' All type components to an entity

        /**
         * @brief Adds a component to an entity.
         * 
         * @tparam ComponentType The type of component.
         * @param e The entity.
         * @param component The component to add.
         * @return The entity (for chaining).
         */
        template<typename ComponentType>
        Entity addComponent(Entity e, const ComponentType &component)
        {
            auto typeID = getComponentID<ComponentType>(); // Fetch the TypeID of this ComponentType
            verifyComponentStorage<ComponentType>(typeID);
            auto *storage = static_cast<StorageComponent<ComponentType> *>(_componentStorages[typeID]); // Get the storage of the componant type
            storage->add(e, component); // Add in storage
            __signatures[e].set(typeID); // Set signature for the new added component
            return e;
        }

        /**
         * @brief Checks if an entity has a specific component.
         * 
         * @tparam ComponentType The component type.
         * @param e The entity.
         * @return True if the entity has the component, false otherwise.
         */
        template<typename ComponentType>
        bool hasComponent(Entity e) const
        {
            assert(e < MAX_ENTITIES);
            auto TypeID = getComponentID<ComponentType>();
            // Signatures are telling if entity SHOULD have a component or not
            // It MAY be possible that an entity has a component set, but signature denies it.
            // Always trust Signature, All composents will be destroyed proprely at ~ECS
            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return false;
            if (!it->second.test(TypeID))
                return false;
            // Signature say entity SHOULD have this, for safety we do internal checks
            if (TypeID >= _componentStorages.size())
                return false;
            if (_componentStorages[TypeID] == nullptr)
                return false;
            auto *storage = static_cast<StorageComponent<ComponentType> *>(_componentStorages[TypeID]);
            return storage->has(e);
        }

        /**
         * @brief Retrieves a read-only pointer to a component.
         * 
         * @tparam ComponentType The component type.
         * @param e The entity.
         * @return Const pointer to the component, or nullptr.
         */
        template<typename ComponentType>
        const ComponentType *getConstComponent(Entity e) const
        {
            auto TypeID = getComponentID<ComponentType>();
            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return nullptr;
            if (!it->second.test(TypeID))
                return nullptr;
            if (TypeID >= _componentStorages.size())
                return nullptr;
            if (_componentStorages[TypeID] == nullptr)
                return nullptr;
            auto *storage = static_cast<const StorageComponent<ComponentType> *>(_componentStorages[TypeID]);
            return storage->get(e);
        }

        /**
         * @brief Retrieves a mutable pointer to a component.
         * 
         * @tparam ComponentType The component type.
         * @param e The entity.
         * @return Pointer to the component, or nullptr.
         */
        template<typename ComponentType>
        ComponentType *getComponent(Entity e)
        {
            auto TypeID = getComponentID<ComponentType>();
            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return nullptr;
            if (!it->second.test(TypeID))
                return nullptr;
            if (TypeID >= _componentStorages.size())
                return nullptr;
            if (_componentStorages[TypeID] == nullptr)
                return nullptr;
            auto *storage = static_cast<StorageComponent<ComponentType> *>(_componentStorages[TypeID]);
            return storage->get(e);
        }

        // THIS IS SO USEFULL FOR SYSTEMS UPDATES + MORE OPTIMIZED FOR THE SYSTEM

        /**
         * @brief Retrieves entities that have all specified components.
         * 
         * @tparam ComponentType Variadic template for component types.
         * @return Vector of entities matching the components.
         */
        template<typename ... ComponentType>
        std::vector<Entity> getEntitiesByComponents()
        {
            std::vector<Entity> validEntities; // Entities having the following ComponentTypes
            Signature requiredSignature; // This is a combinaison of all required component in one signature.
            // This will be compared to the Entity signature with bit manipulation
            // This will set bits of the required components
            (requiredSignature.set(getComponentID<ComponentType>()), ...);
            for (auto &[entity, signature] : __signatures)
                // Check if entity has all required components
                if ((signature & requiredSignature) == requiredSignature)
                    validEntities.push_back(entity);
            // Could to a _cacheQuery later
            // Stock last vector<Entity> and return it in case of same request with requiredSignature
            return validEntities;
        }

        /**
         * @brief Removes a specific component from an entity.
         * 
         * @tparam ComponentType The component type.
         * @param e The entity.
         */
        template<typename ComponentType>
        void killComponent(Entity e)
        {
            auto TypeID = getComponentID<ComponentType>();
            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return;
            if (!it->second.test(TypeID))
                return;
            if (TypeID >= _componentStorages.size())
                return;
            if (_componentStorages[TypeID] == nullptr)
                return;
            auto *storage = static_cast<StorageComponent<ComponentType> *>(_componentStorages[TypeID]);
            storage->kill(e);
            __signatures[e].reset(TypeID);
        }

        /**
         * @brief Gets the current time in the ECS.
         * 
         * @return The current time.
         */
        double getTime() const { return _time; }

        /**
         * @brief Adds delta time to the ECS time.
         * 
         * @param dt The delta time to add.
         */
        void addTime(double dt) { _time += dt; }

    private:
        double _time;
        std::size_t __livingEntities = 0;
        // The entities not used yet
        std::queue<Entity> __freeEntities;
        // Bitmask used for linking entiteis to their component
        std::unordered_map<Entity, Signature> __signatures;
        std::vector<void *> _componentStorages; // void* are std::Vector<ComponentType> from storageComponent class
        // CHANGE void * INTO IComponentStorage and unique_ptr later
        // Also change the static_cast once IComponent created (std::vector<std::unique_ptr<IComponentStorage>>)

        /**
         * @brief Verifies and initializes storage for a component type if needed.
         * 
         * @tparam ComponentType The component type.
         * @param TypeID The ID of the component type.
         */
        template<typename ComponentType>
        void verifyComponentStorage(ComponentTypeID TypeID)
        {
            // Update types storage size if full and add them if not existing
            // Ensure that _componentStorage is pointing toward a StorageComponent instance
            // Resize the vector if storage is full
            if (TypeID >= _componentStorages.size())
                _componentStorages.resize(TypeID + 1, nullptr);
            // Add a new Component Type Storage if not existing at TypeID
            if ( _componentStorages[TypeID] == nullptr)
                _componentStorages[TypeID] = new StorageComponent<ComponentType>();
        }
};
// DONT FORGET TO DESTROY PROPRELY COMPONENTS IN ~ECS()
// Right now: Always Trusting signature "Inshallah"

// ### ISYSTEM CLASS ###

/**
 * @class ISystem
 * @brief Interface for systems in the ECS.
 * 
 * Systems update components at a given delta time.
 */
class ISystem
{
    public:
        /**
         * @brief Constructor for ISystem.
         * 
         * @param ecs Reference to the ECS instance.
         */
        ISystem(ECS &ecs)
            : _ecs(ecs) {};

        /**
         * @brief Virtual destructor for ISystem.
         */
        virtual ~ISystem() = default;

        /**
         * @brief Pure virtual function to update the system.
         * 
         * @param dt Delta time for the update.
         */
        virtual void update(double dt) = 0; // Double is 2 more precise than a float

    protected:
        ECS &_ecs;
};

#endif /* ECS_HPP */