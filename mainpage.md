@mainpage
# Documentation de la Game Engine dans le Projet R-Type

Cette documentation se concentre sur la game engine du projet R-Type, en particulier sur l'Entity Component System (ECS), les systèmes (systems), et leur utilisation dans le fichier `main.cpp` du client. L'objectif est de clarifier les relations entre ces éléments, en expliquant comment l'ECS gère les entités, les composants et les systèmes, et comment tout cela est orchestré pour créer un jeu de type shoot'em up.

La game engine est implémentée dans les répertoires `include/engine` et `src/engine`, avec une séparation claire entre le core (ECS), les factories (pour créer des entités), et les systems (pour la logique du jeu). Le server gère le réseau UDP, mais la game engine est principalement utilisée côté client pour la logique de jeu, le rendering et les inputs.

## 1. Introduction à la Game Engine

La game engine est basée sur un paradigme ECS (Entity-Component-System), qui est une architecture courante en développement de jeux pour favoriser la composition plutôt que l'héritage. Cela permet une flexibilité accrue : les entités sont des IDs simples, les composants stockent les données, et les systèmes appliquent la logique.

- **Entity** : Un simple ID (`uint32_t`) représentant un objet dans le jeu (ex. : joueur, ennemi, projectile).
- **Component** : Des données attachées à une entité (ex. : Position, Velocity, Health).
- **System** : Des classes qui itèrent sur les entités ayant certains composants et appliquent des mises à jour (ex. : mouvement, collisions, rendering).

L'ECS est défini dans `ecs.hpp` et utilisé dans le `main.cpp` du client pour initialiser le jeu, créer des entités via des factories, et boucler sur les updates des systèmes.

### Relations clés :
- L'ECS est le "coeur" : il stocke les entités, composants et systèmes.
- Les factories (`EntityFactory.hpp`, `StageFactory.hpp`) créent des entités pré-configurées avec des composants.
- Les systèmes héritent de `ISystem` et sont ajoutés à l'ECS pour être exécutés dans la boucle de jeu.
- Dans `main.cpp`, tout est assemblé : création de l'ECS, ajout de systèmes, initialisation d'entités, et boucle update/render.

## 2. L'Entity Component System (ECS)

L'implémentation de l'ECS est dans `ecs.hpp`. C'est une implémentation légère, optimisée pour un maximum de 2048 entités (`MAX_ENTITIES`) et 128 types de composants (`MAX_COMPONENT`).

### Concepts Clés de l'ECS :
- **Entity** : Un ID unique (`std::uint32_t`). Les entités sont créées via `createEntity()` et détruites via `killEntity()`. Une queue recycle les IDs des entités détruites pour éviter la fragmentation.
- **Signature** : Un bitset (`std::bitset<MAX_COMPONENT>`) qui indique quels composants une entité possède. Cela permet des requêtes rapides comme `getEntitiesByComponents<T1, T2...>()`.
- **ComponentTypeID** : Un ID unique pour chaque type de composant, généré via `getComponentID<T>()` (utilise un compteur statique).
- **StorageComponent<T>** : Une classe template qui stocke les composants d'un type donné pour toutes les entités (vecteur de taille `MAX_ENTITIES`). Elle utilise un bitset pour tracker quelles entités ont ce composant.
- **ECS Class** : La classe principale qui gère tout :
  - Stocke les entités vivantes et une queue pour recycler les IDs.
  - Un vecteur de pointeurs vers des storages de composants (`std::vector<IComponentStorage*>`).
  - Un vecteur de systèmes (`std::vector<std::unique_ptr<ISystem>>`).
  - Méthodes clés :
    - `createEntity()` : Alloue un nouvel ID.
    - `addComponent<T>(Entity e, T component)` : Ajoute un composant à une entité, met à jour la signature.
    - `getComponent<T>(Entity e)` : Récupère un composant (pointeur).
    - `getEntitiesByComponents<T1, T2...>()` : Retourne les entités ayant tous les composants spécifiés (via signatures).
    - `addSystem(std::unique_ptr<ISystem> system)` : Ajoute un système.
    - `update(double dt)` : Appelle `update(dt)` sur tous les systèmes.

### Relations dans l'ECS :
- **Entité → Composants** : Une entité n'a pas de données intrinsèques ; elle référence des composants via son ID et sa signature.
- **Composants → Storage** : Chaque type de composant a son propre storage (vector<T>), indexé par l'ID d'entité.
- **Systèmes → ECS** : Les systèmes ont une référence à l'ECS (`&_ecs`) pour accéder aux entités et composants via des queries comme `getEntitiesByComponents()`.
- **Gestion de la Mémoire** : Les storages sont dynamiquement alloués quand un nouveau type de composant est ajouté. La destruction des entités met à jour les signatures mais ne libère pas immédiatement les composants (confiance en la signature).

### Exemple de flux :
- Créer une entité : `Entity e = ecs.createEntity();`
- Ajouter un composant : `ecs.addComponent(e, Position{0.f, 0.f});` → Met à jour la signature de l'entité et stocke dans `StorageComponent<Position>`.
- Query : `auto entities = ecs.getEntitiesByComponents<Position>();` → Itère sur les signatures pour trouver les matches.

## 3. Les Composants

Les composants sont des structs simples définis dans `Components.hpp`. Ils représentent des données pures, sans logique.

### Exemples principaux :
- **Position** : `{float x, y}` – Position dans l'espace 2D.
- **Velocity** : `{float x, y}` – Vitesse de déplacement.
- **Drawable** : `{std::string textureId, sf::IntRect rect, int layer, bool visible, float scale, float rotation}` – Pour le rendering.
- **PlayerController** : `{uint8_t playerId, bool isShooting}` – Contrôles du joueur.
- **Collider** : `{float width, height, bool solid, uint8_t team, int damage}` – Pour les collisions.
- **Health** : `{int current, int max}` – Points de vie.
- **Shootable** : `{float cooldown, float shootDelay, float missileSpeed, int damage, uint8_t team, std::string textureId, float offsetX, float offsetY, bool isShooting, bool tripleShot}` – Pour les tirs.
- **MovementPattern** : `{Type type (enum: Linear, Sinus, etc.), float amplitude, frequency, radius, speed}` – Patterns de mouvement pour ennemis.
- **Projectile** : `{float velocity, int damage}` – Pour les missiles.
- **PowerUp** : `{Type type (ex. TripleShot), float duration}` – Bonus.
- **Obstacle / DestructibleTile / Star** : Spécifiques au stage/background.

### Relations :
- Les composants sont attachés aux entités via l'ECS.
- Les systèmes itèrent sur des combinaisons spécifiques de composants (ex. : `MovementSystem` sur `<Position, Velocity, MovementPattern>`).

## 4. Les Systèmes

Les systèmes héritent de `ISystem` (dans `ecs.hpp`), qui définit `virtual void update(double dt) = 0;`. Chaque système accède à l'ECS pour query les entités pertinentes et updater leurs composants.

### Liste des systèmes (dans `systems/` ) :
- **InputSystem** : Gère les inputs clavier/manette. Mappe des actions (MoveUp, Shoot, etc.) et applique aux entités avec `<PlayerController, Velocity, Shootable>`. Relations : Lit SFML events, met à jour Velocity et isShooting.
- **MovementSystem** : Met à jour `<Position>` basé sur `<Velocity>` et `<MovementPattern>` (pour patterns comme Sinus, Zigzag). Relations : Applique dt à la position, avec modulations pour patterns ennemis.
- **CollisionSystem** : Détecte et résout collisions AABB sur `<Position, Collider>`. Gère dommages via `<Health, Projectile>`. Relations : Itère sur paires d'entités, résout overlaps, applique dégâts, détruit entités (`ecs.killEntity`).
- **MissileSystem** : Gère les tirs via `<Shootable, Position>`. Crée projectiles (via `EntityFactory`), gère cooldowns et tripleShot. Relations : Utilise factories pour spawn, supprime hors-écran.
- **PowerUpSystem** : Gère collisions power-ups avec joueurs, active effets comme tripleShot sur `<Shootable>`. Relations : Détruit power-up après collision.
- **WaveSystem** : Spawn vagues d'ennemis basé sur WaveData (delay, count, etc.). Crée ennemis via `ECS.addComponent`. Relations : Timer-based, utilise factories implicitement.
- **RenderSystem** : Dessine tout via SFML sur `<Position, Drawable, Star, etc.>`. Tri par layers. Relations : Accède à `ResourceManager` pour textures, clear/draw/display sur window.

### Autres :
- **ResourceManager** : Singleton pour charger textures (non un système ECS, mais utilisé par `RenderSystem`).

### Relations générales des systèmes :
- Tous les systèmes sont ajoutés à l'ECS via `ecs.addSystem(std::make_unique<System>(ecs, ...));`.
- Dans `update(dt)`, chaque système query des entités spécifiques et modifie leurs composants.
- Ordre d'exécution important : Typiquement Input → Movement → Collision → Missile/PowerUp/Wave → Render.

## 5. Utilisation dans le `main.cpp` (Client)

Le `main.cpp` du client (dans `src/client`) assemble tout :

### Initialisation :
- Crée `sf::RenderWindow` (1920x1080 fullscreen).
- Crée `ECS ecs`.
- Charge textures via `ResourceManager` (singleton).
- Ajoute systèmes : `InputSystem`, `MovementSystem`, `CollisionSystem`, `MissileSystem`, `PowerUpSystem`, `WaveSystem`, `RenderSystem` (certains avec params comme window).

### Setup du Stage :
- Crée starfield via `Factory::createStarfield(ecs, ...)`.
- Crée borders via `Factory::createScreenBorders(ecs, ...)`.
- Crée joueur via `Factory::createPlayer(ecs, ...)`.
- Charge level waves via `WaveSystem.loadLevel(...)`.

### Boucle de Jeu :
- `sf::Clock` pour dt.
- Tant que `window.isOpen()` :
  - Gère events SFML (close, etc.).
  - Appelle `ecs.update(dt)` → Exécute tous les systèmes.
  - Mises à jour manuelles (ex. : wrap étoiles, delete projectiles hors-écran).
  - Render via `render.update(dt)` (mais déjà dans `ecs.update` ? Attends, render est un système, mais code montre un appel manuel ? Dans le code truncaté, il y a des updates manuelles après `ecs.update`, mais idéalement tout via systèmes).

### Relations dans `main` :
- ECS est central : Factories l'utilisent pour créer/add composants.
- Boucle : dt passé à `ecs.update` → Propagé à chaque système.
- SFML intégré : Window passé à `RenderSystem`, events à `InputSystem`.

## 6. Relations Globales et Flux

- **Flux Typique** : Main → ECS (add systems/factories) → Boucle (update dt) → Systèmes (query entités/composants via ECS) → Modif composants → Render.
- **Dépendances** : Systèmes dépendent de composants (queries). Factories configurent entités avec composants spécifiques pour matcher les queries des systèmes.
- **Avantages ECS** : Scalable, facile d'ajouter systèmes/composants sans toucher le core.
- **Points d'Amélioration** : Ordre des systèmes non garanti (vecteur), pas de multithreading, confiance en signatures (risque de corruption).

# Bienvenue dans la Documentation de R-Type

Ce projet est une implémentation d'un shoot'em up inspiré de R-Type, utilisant un ECS (Entity Component System) pour la game engine côté client, et un serveur UDP pour le réseau.

## Aperçu Général
- **Client** : Gère le rendering (SFML), inputs, mouvements, collisions, etc. via ECS.
- **Serveur** : Gère le réseau (UDP), interprétation de paquets, et boucle de jeu.
- **Architecture** : Basée sur ECS pour flexibilité ; voir la hiérarchie des classes pour les systèmes héritant de `ISystem`.

## Sections Principales
- [Hiérarchie des classes](hierarchy.html) : Vue graphique des héritages (ex. systèmes → `ISystem`).
![Hiérarchie des classes - Graphe détaillé](class_graph.png)
*Description : Graphe UML montrant les relations entre ECS, ISystem, les systèmes et les composants.*
- [Espaces de nommage](namespaces.html) : Détails sur `Components` et autres.
- [Fichiers](files.html) : Liste des sources.
- [Modules](modules.html) : Groupes comme les composants ECS.

## Comment Naviguer
Utilisez la barre de recherche en haut pour trouver des classes spécifiques (ex. `ECS`, `MovementSystem`).

Pour plus de détails sur l'ECS, voir [Classe ECS](classECS.html).

## Contact
Projet Epitech 2025 - Gabriel VILLEMONTE - Mathis COUTAYE - Clement CHELLIER - Lucas HOAREAU