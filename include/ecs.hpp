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

static constexpr size_t MAX_ENTITIES = 1 << 11; // 2048 Entities Max (Enough for our project)
static constexpr size_t MAX_COMPONENT = 1 << 7; // 128 Components max par Entites

using Entity = std::uint32_t; // Entity is just an ID
using Signature = std::bitset<MAX_COMPONENT>; // The checker for an entity has a certain component
using ComponentTypeID = std::size_t; // TypeID's for ComposentType


// ### COMPONENT TYPE ID ###

// Create a new type of component
inline ComponentTypeID newComponentTypeID()
{
    static ComponentTypeID last = 0;
    return last++;
}

template <typename T>
ComponentTypeID getComponentID()
{
    static ComponentTypeID id = newComponentTypeID();
    return id;
}


// ### STORAGE COMPONENTS ###

// Used for each type of component. Stock every component w the same type together.
template<typename ComponentType>
class StorageComponent
{
    public:
        StorageComponent()
        {
            __Storage.resize(MAX_ENTITIES); // Posibly one for every entity
            __checker.reset();
        }
        ~StorageComponent() = default;

        // FUCK storage : Find, Update, Create & Kill for component
        void add(Entity e, const ComponentType &component)
        {
            assert(e < MAX_ENTITIES);

            __Storage[e] = component; //component for 'e' entity is in [e]
            __checker.set(e); // Update checker
        }

        bool has(Entity e) const
        {
            assert(e < MAX_ENTITIES);

            return __checker.test(e); // Check 'e' bit to see if 'e' entity has this component type
        }

        const ComponentType *get(Entity e) const // READ-ONLY version because const (FIND)
        {
            assert(e < MAX_ENTITIES);
            if (!has(e))
                return nullptr;

            return &__Storage[e]; // Reference to the component to modify it IN SYSTEM ONLY
        }

        ComponentType *get(Entity e) // get version for UPDATE
        {
            assert(e < MAX_ENTITIES);
            if (!has(e))
                return nullptr;

            return &__Storage[e];
        }

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

class ECS
{
    public:
        ECS()
        {
            for (Entity e = 0; e < MAX_ENTITIES; e++)
                __freeEntities.push(e);

            assert(!__freeEntities.empty());
        }
        ~ECS() = default;


        // ## 'FUCK' all entities ##

        // Create an entity. Remove a free entity and count it as a living one.
        Entity createEntity()
        {
            assert(__livingEntities < MAX_ENTITIES);

            Entity id = __freeEntities.front();
            __freeEntities.pop();
            __livingEntities++;
            return id;
        }

        void killEntity(Entity e)
        {
            assert(e < MAX_ENTITIES);

            __signatures[e].reset();
            __freeEntities.push(e);
            __livingEntities--;
        }

        Signature getSignature(Entity e) const
        {
            assert(e < MAX_ENTITIES);

            auto it = __signatures.find(e);
            if (it == __signatures.end())
                return Signature(); // An empty bitset
            return it->second; // The Entity is first in the unordered map, their signature is second
        }

        void setSignature(Entity e, Signature sig)
        {
            assert(e < MAX_ENTITIES);

            __signatures[e] = sig;
        }


        // ### Composant manager ###

        // 'FUCK' All type components to an entity
        template<typename ComponentType>
        Entity addComponent(Entity e, const ComponentType &component)
        {
            auto typeID = getComponentID<ComponentType>(); // Fetch the TypeID of this ComponentType
            verifyComponentStorage<ComponentType>(typeID);

            auto *storage = static_cast<StorageComponent<ComponentType> *>(_componentStorages[typeID]); // Get the storage of the componant type
            storage->add(e, component);     // Add in storage
            __signatures[e].set(typeID);    // Set signature for the new added component

            return e;
        }

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


    private:
        std::size_t __livingEntities = 0;

        // The entities not used yet
        std::queue<Entity> __freeEntities;

        // Bitmask used for linking entiteis to their component
        std::unordered_map<Entity, Signature> __signatures;

        std::vector<void *> _componentStorages; // void* are std::Vector<ComponentType> from storageComponent class
        // CHANGE void * INTO IComponentStorage and unique_ptr later
        // Also change the static_cast once IComponent created (std::vector<std::unique_ptr<IComponentStorage>>)

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

// ISystem interface for creating systems
// system are used to update component at a certain delta time (dt)
// It performs logic and calculations of components at dt.
class ISystem
{
    public:
        ISystem(ECS &ecs)
            : _ecs(ecs) {};
        virtual ~ISystem() = default;

        virtual void update(double dt) = 0; // Double is 2 more precise than a float

    protected:
        ECS &_ecs;
};

#endif /* ECS_HPP */