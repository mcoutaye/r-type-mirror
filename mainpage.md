Module Réseau - R-Type Mirror {#mainpage}
=====================================

![Vue d'ensemble du module réseau](assets/network_overview.png "Vue d'ensemble du module réseau"){ width=800px }

# Module Réseau du projet R-Type Mirror

Bienvenue dans la documentation du module réseau du projet **r-type-mirror**,
un jeu multijoueur inspiré de R-Type développé dans le cadre d'Epitech 2025.

Ce module gère toute la communication réseau entre le serveur et les clients avec une architecture
**hybride TCP/UDP** optimisée pour un gameplay fluide et réactif.

## Architecture générale

Le système réseau est séparé en deux canaux complémentaires :

- **TCP** (port par défaut : **53001**) :
  Connexion fiable pour la découverte, l'authentification légère et le keep-alive.
  Protocole simple ping/pong avec détection de timeout.

- **UDP** (port par défaut : **53000**) :
  Gameplay en temps réel (faible latence, tolérance aux pertes de paquets).
  Envoi des inputs joueur → serveur.
  Broadcast des positions d'entités serveur → clients.

## Namespace

Tout le code réseau est encapsulé dans le namespace `Nwk` pour éviter les conflits.

## Classes principales

| Classe                          | Rôle principal                                   | Canal |
|---------------------------------|--------------------------------------------------|-------|
| [@ref Nwk::TcpServer]           | Gestion des connexions et keep-alive             | TCP   |
| [@ref Nwk::TcpClient]           | Connexion initiale et maintien de la liaison     | TCP   |
| [@ref Nwk::UdpServer]           | Réception inputs + envoi snapshots entités       | UDP   |
| [@ref Nwk::UdpClient]           | Envoi inputs + réception snapshots               | UDP   |
| [@ref Nwk::SafeQueue]           | Communication thread-safe entre réseau et logique jeu | -     |

## Protocole UDP (gameplay)

- **InputState** (1 octet) : bitfield des inputs (haut, bas, gauche, droite, tir)
- **EntityUpdate** (12 octets) : { entityId (uint32), x (float), y (float) }
- Les snapshots sont envoyés sous forme de multiples EntityUpdate concaténés

## Fonctionnalités clés

- Ajout automatique des clients UDP à la réception du premier input
- Limitation à **4 joueurs maximum** (MAX_CLIENTS = 4)
- Threads dédiés avec arrêt propre et join
- Files thread-safe pour découpler réseau et game loop
- Détection de déconnexion via timeout TCP

## Navigation rapide

- [@ref Nwk "Namespace Nwk"] → Toutes les classes du module
- [@ref Nwk::UdpServer "UdpServer"] → Cœur du gameplay serveur
- [@ref Nwk::UdpClient "UdpClient"] → Côté client gameplay

---

*Projet réalisé par Gabriel Villemonte & Clément Chellier – Epitech 2025*
