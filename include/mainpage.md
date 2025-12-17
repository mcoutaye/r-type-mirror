@mainpage Documentation du Projet R-Type

# Bienvenue dans la Documentation de R-Type

Ce projet est une implémentation d'un shoot'em up inspiré de R-Type, utilisant un ECS (Entity Component System) pour la game engine côté client, et un serveur UDP pour le réseau.

## Aperçu Général
- **Client** : Gère le rendering (SFML), inputs, mouvements, collisions, etc. via ECS.
- **Serveur** : Gère le réseau (UDP), interprétation de paquets, et boucle de jeu.
- **Architecture** : Basée sur ECS pour flexibilité ; voir la hiérarchie des classes pour les systèmes héritant de ISystem.

## Sections Principales
- [Hiérarchie des classes](hierarchy.html) : Vue graphique des héritages (ex. systèmes → ISystem).
- [Espaces de nommage](namespaces.html) : Détails sur `Components` et autres.
- [Fichiers](files.html) : Liste des sources.
- [Modules](modules.html) : Groupes comme les composants ECS.

## Comment Naviguer
Utilisez la barre de recherche en haut pour trouver des classes spécifiques (ex. `ECS`, `MovementSystem`).

Pour plus de détails sur l'ECS, voir [Classe ECS](classECS.html).

@note Cette doc est générée avec Doxygen. Pour contribuer, ajoutez des commentaires /** */ dans le code.

## Contact
Projet Epitech 2025 - Gabriel VILLEMONTE - Mathis COUTAYE - Clement CHELLIER - Lucas HOAREAU