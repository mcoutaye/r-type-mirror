# R-Type

A modern multiplayer remake of the classic R-Type game, built with C++20 and SFML using an Entity Component System (ECS) architecture.

## Description

R-Type is a horizontal scrolling shooter game where players control a spaceship fighting against waves of enemies. This project implements both a game client with graphics and input handling, and a networked game server for multiplayer gameplay.

## 🛠️ Build Requirements

- **CMake** >= 3.20
- **C++ Compiler** with C++20 support (GCC 10+, Clang 11+, MSVC 2019+)
- **SFML** 2.6.1 (automatically fetched via CMake)

### Linux Dependencies
On Ubuntu/Debian, the following will be auto-installed if missing:
```bash
libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev
libudev-dev libopenal-dev libgl1-mesa-dev libfreetype6-dev
libvorbis-dev libflac-dev
```

## Building

```bash
# Clone the repository
git clone <repository-url>
cd r-type-mirror

# Configure and build
cmake -B build
cmake --build build

# Executables will be in:
# - build/src/client/r-type_client
# - build/src/server/r-type_server
```

## 👥 Authors

- [Lucas Hoarau](https://github.com/Leukos974)
- [Mathis Coutaye](https://github.com/mcoutaye)
- [Gabriel Villemonte](https://github.com/DarkoGabi)
- [Clément Chellier](https://github.com/Astromind974)
