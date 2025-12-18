# Proof of Concept: Choice of SFML for R-Type Project

## 1. Introduction
This document outlines the technical reasoning behind choosing **SFML (Simple and Fast Multimedia Library)** for the R-Type project, specifically focusing on architectural fit and networking capabilities compared to major alternatives (SDL2, Raylib, Qt).

## 2. Comparison with Alternatives

### A. SFML vs. SDL2 (Simple DirectMedia Layer)
*   **Language Paradigm:** SDL2 is a **C library**. It requires manual memory management (destroying textures/windows) and pointer handling. SFML is a **C++ library** that uses RAII (Resource Acquisition Is Initialization), meaning resources are automatically cleaned up when they go out of scope.
*   **Networking:** SDL2 is strictly a media layer (Graphics, Audio, Input). It **does not** have a networking module. To use SDL2, we would need to integrate a separate, complex library like `Boost.Asio` or raw OS sockets.
*   **Architecture:** SFML's Object-Oriented design (`sf::Sprite`, `sf::Texture`) aligns perfectly with the C++ Class-based architecture of our ECS (Entity Component System). SDL2's procedural C style would require writing extensive C++ wrappers.

### B. SFML vs. Raylib
*   **Rendering Paradigm:** Raylib often uses "Immediate Mode" rendering (drawing commands issued every frame procedurally). While powerful for prototyping, this can conflict with the strict state management required in a distributed multiplayer game. SFML's "Retained Mode" style (configuring objects and drawing them) fits better with persistent game entities.
*   **Networking:** Like SDL2, Raylib is primarily for graphics and audio. It lacks a robust, built-in object-oriented networking class compatible with C++ streams.
*   **Language:** Raylib is C-based. Using it in a C++20 project means losing out on modern C++ features (references, classes, templates) in the graphics layer.

### C. SFML vs. Qt (Qt 6)
*   **Performance:** Qt is a heavy application framework designed for GUIs, not real-time arcade games. Its event loop and Signal/Slot mechanism introduce significant overhead compared to SFML's lightweight `pollEvent` loop.
*   **Architecture Control:** Qt enforces a strict architecture (`QObject` hierarchy, `moc` compiler). SFML is "passive," allowing us to build our own custom architecture (like the ECS in `include/ecs.hpp`) without framework interference.
*   **Complexity:** Qt includes tools for everything (WebEngine, SQL, XML), which is unnecessary bloat for a game client/server.

## 3. Why SFML is the Superior Choice for R-Type

### A. Unified Networking & Graphics
SFML is unique among lightweight libraries because it provides **Graphics, Audio, and Networking** in a single cohesive package.
*   **Benefit:** We do not need to manage two separate dependencies (e.g., SDL for graphics + Boost for network).
*   **Synergy:** We can use `sf::IpAddress` and `sf::UdpSocket` on the server, and `sf::RenderWindow` on the client, all sharing the same style and conventions.

### B. Protocol & Serialization (`sf::Packet`)
*   **Endianness:** SFML's `sf::Packet` automatically handles "Endianness" (byte order) conversion. This ensures that a Linux Server and a Windows Client can communicate integers and floats without data corruption.
*   **Ease of Use:** We can stream data directly into packets (`packet << x << y;`) without complex pointer arithmetic or `memcpy` operations required by raw sockets.

### C. Modern C++ Compatibility
*   Our project uses **C++20**. SFML is written in C++ and supports modern idioms naturally.
*   It integrates seamlessly with `std::shared_ptr` and `std::vector`, which are heavily used in our ECS and Entity Factory.

### D. Development Velocity
*   SFML abstracts away low-level OpenGL and Socket API calls, allowing the team to focus on Game Logic (ECS, Physics, Wave Systems) rather than boilerplate code.

## 4. Architectural Study: ECS vs. OOP

### A. Traditional OOP (Object-Oriented Programming)
*   **Structure:** Uses deep inheritance hierarchies (e.g., `GameObject` -> `Movable` -> `Enemy` -> `Boss`).
*   **The Problem (Diamond of Death):** If we want an entity that is both a "Building" and "Movable" (like a flying fortress), we run into multiple inheritance issues.
*   **Memory:** Objects are often scattered in heap memory, causing "cache misses" as the CPU jumps around to find data.
*   **Coupling:** Logic (methods) and Data (member variables) are tightly bound in the same class.

### B. ECS (Entity Component System) - Chosen for R-Type
*   **Structure:**
    *   **Entity:** Just a unique ID (uint32).
    *   **Component:** Pure data (Position, Velocity, Sprite, Health). No logic.
    *   **System:** Pure logic (MovementSystem, RenderSystem). Iterates over entities that have specific components.
*   **Composition over Inheritance:** We build entities by "plugging in" components. A "Flying Fortress" is just an Entity with `Position`, `Sprite`, and `Velocity` components. No complex inheritance needed.
*   **Performance (Data Locality):** Components of the same type are stored in contiguous arrays. This allows the CPU to prefetch data efficiently, drastically improving performance for games with many objects (like bullets in R-Type).
*   **Network Serialization:** Since Components are POD (Plain Old Data) structs, they are trivial to serialize and send over the network compared to complex OOP objects with vtables.

## 5. Algorithms & Data Structures

### A. Algorithms
*   **Collision Detection (AABB):** We use Axis-Aligned Bounding Box collision detection (`sf::FloatRect::intersects`). This is an $O(N^2)$ algorithm in its simplest form, iterating through entity pairs. While simple, it is sufficient for the entity count in R-Type (< 2048).
*   **System Iteration:** Systems iterate over entities matching a specific `Signature` (Bitset). This is an $O(N)$ operation where N is the number of entities with that specific component combination.

### B. Data Structures
*   **Bitsets (`std::bitset`):** Used for Entity Signatures to quickly check if an entity has the required components for a system (O(1) check).
*   **Contiguous Arrays (`std::vector`):** Components are stored in dense arrays to maximize cache locality.
*   **Thread-Safe Queues (`SafeQueue`):** A custom thread-safe queue using `std::mutex` and `std::condition_variable` is used to buffer network packets between the UDP reception thread and the main game loop.

## 6. Network Protocol & Reliability

### A. Hybrid Protocol Strategy
*   **UDP (User Datagram Protocol):** Used for high-frequency, low-latency data:
    *   **Client -> Server:** `InputState` (Player inputs).
    *   **Server -> Client:** `EntityUpdate` (Positions, Health).
    *   **Reliability:** We accept packet loss for movement data because a newer packet will arrive ~16ms later (60Hz), making the old one obsolete.
*   **TCP (Transmission Control Protocol):** Used for critical, low-frequency events:
    *   Connection/Disconnection.
    *   Lobby Management.
    *   Reliability is guaranteed by TCP itself.

### B. Protocol Documentation (RFC Style)
The protocol is documented in `rfcprotocol.txt`, defining packet structures in Big-Endian format.
*   **InputState:** 3 bytes (ID + Buttons).
*   **EntityUpdate:** Packed struct with ID, X, Y coordinates.

## 7. Security & Data Integrity

### A. Vulnerabilities
*   **UDP Spoofing:** Since UDP is connectionless, an attacker could forge packets with a victim's Player ID.
*   **Replay Attacks:** Capturing valid packets and resending them to simulate movement.
*   **Plaintext:** Data is currently sent unencrypted.

### B. Mitigation & Monitoring
*   **Source Validation:** The server validates that the `sf::IpAddress` and Port of the incoming UDP packet match the registered client for that Player ID.
*   **Future Security (Long Term):**
    *   **Sequence Numbers:** To prevent replay attacks, we will implement a rolling sequence number in the UDP header.
    *   **Token Authentication:** A session token exchanged via TCP during handshake will be required in UDP packets.
    *   **Sanity Checks:** The server enforces game logic (e.g., max speed) to prevent "speed hacks" even if the client sends modified coordinates.

## 8. Documentation Strategy
*   **Technical Documentation:** We use Markdown (`.md`) files in the repository for architectural decisions (like this POC).
*   **Protocol Documentation:** The network protocol is strictly defined in `rfcprotocol.txt` to ensure any client (even non-SFML ones) can implement the standard.
*   **Code Documentation:** Doxygen-style comments are used in headers (`.hpp`) to describe classes and methods.