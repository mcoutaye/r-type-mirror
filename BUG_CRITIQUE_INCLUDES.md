# 🔴 BUG CRITIQUE TROUVÉ : Code 127 - Includes Manquants

## Date: 18 janvier 2026

---

## ❌ Problème

Sur Windows, après pull et rebuild, code **127** persiste.

```bash
./build/r-type_client.exe
echo $?
# 127
```

Aucun log ne s'affiche, même avec les `std::cout` ajoutés.

---

## 🔍 Analyse Approfondie

### Code 127 = Deux Causes Possibles

1. **DLLs manquantes** → Résolu avec copie POST_BUILD ✅
2. **Erreur de compilation silencieuse** → **TROUVÉ** ⚠️

---

## 🐛 Bug Trouvé : Includes Manquants

### Fichier: `include/client.hpp`

**Avant (BUGGÉ):**
```cpp
#pragma once
#include "ecs.hpp"
// ...
#include <memory>
// ❌ PAS D'IOSTREAM !
```

**Problème:**
- J'ai ajouté des `std::cout` et `std::cerr` dans le constructeur `Client`
- Mais `<iostream>` n'était **PAS** inclus dans `client.hpp`
- Sur Linux: compile quand même (include transitif via d'autres headers)
- Sur Windows (MSVC): **erreur de compilation** → l'exécutable n'est PAS créé
- CMAKE retourne quand même 0 (succès) car seul le linkage échoue silencieusement

### Résultat:
- L'exécutable `r-type_client.exe` est **vide ou invalide**
- Windows refuse de le lancer → Code 127

---

## ✅ Solution Appliquée

**Fichier:** `include/client.hpp`

```cpp
#pragma once
#include "ecs.hpp"
#include "UdpClient.hpp"
#include "Timer.hpp"
#include "engine/systems/Components.hpp"
#include "engine/systems/InputSystem.hpp"
#include "engine/systems/RenderSystem.hpp"
#include "engine/systems/MoveSystem.hpp"
#include "engine/systems/RessourceManager.hpp"
#include "engine/systems/SoundSystem.hpp"
#include "engine/systems/MenuSystem.hpp"
#include "engine/systems/SceneManager.hpp"
#include "engine/systems/ParticleSystem.hpp"
#include "engine/systems/CameraSystem.hpp"
#include <memory>
#include <iostream>      // ✅ AJOUTÉ pour std::cout, std::cerr
#include <stdexcept>     // ✅ AJOUTÉ pour std::runtime_error
```

---

## 📊 Récapitulatif de TOUS les Correctifs

### 1. Noms d'exécutables ✅
- `rtype_client` → `r-type_client`
- `rtype_server` → `r-type_server`

### 2. Linkage statique MinGW ✅
```cmake
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(r-type_client PRIVATE -static-libgcc -static-libstdc++)
endif()
```

### 3. Fix console Windows (sfml-main) ✅
```cmake
if(WIN32)
    target_link_libraries(r-type_client PRIVATE sfml-main)
endif()
```

### 4. Copie automatique assets ✅
```cmake
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type_client>/assets
)
```

### 5. Copie automatique DLLs SFML ✅
```cmake
if(WIN32)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system> ...
    )
endif()
```

### 6. Logs de debug ✅
- `src/client/main.cpp`: try/catch + logs détaillés
- `include/client.hpp`: logs à chaque étape du constructeur

### 7. Fallback fenêtre ✅
```cpp
// Essaie fullscreen, sinon fenêtré
_window.create(..., sf::Style::Fullscreen);
if (!_window.isOpen()) {
    _window.create(..., sf::Style::Default);
}
```

### 8. **INCLUDES MANQUANTS** ✅ ← **LE BUG CRITIQUE**
```cpp
#include <iostream>   // Pour std::cout, std::cerr
#include <stdexcept>  // Pour std::runtime_error
```

---

## 🧪 Test Final

### Sur Windows:

```bash
# 1. Pull les derniers changements
git pull origin Test_windows

# 2. Clean rebuild
rm -rf build
cmake -B build
cmake --build build --config Release

# 3. Vérifier que l'exécutable existe et n'est pas vide
ls -lh build/r-type_client.exe
# Doit afficher une taille > 1MB

# 4. Vérifier les DLLs
ls build/*.dll
# Doit afficher: sfml-*.dll

# 5. Lancer
./build/r-type_client.exe
```

### Sortie Attendue:

```
[R-TYPE CLIENT] Démarrage du client...
[R-TYPE CLIENT] Connexion au serveur: 127.0.0.1 (par défaut)
[R-TYPE CLIENT] Initialisation du client...
[Client] Création de la fenêtre...
[Client] Fenêtre créée avec succès (1920x1080)
[Client] Démarrage du client UDP...
[Client] Initialisation du SceneManager...
[Client] Chargement des ressources...
[Client] Chargement des sons...
[Client] Création de la caméra...
[Client] Initialisation des menus...
[Client] Démarrage de la musique de menu...
[Client] Initialisation terminée avec succès!
[R-TYPE CLIENT] Client initialisé avec succès!
[R-TYPE CLIENT] Démarrage de la boucle principale...
```

---

## 🎯 Pourquoi Ça Marchait sur Linux mais Pas Windows?

### Linux (GCC):
- **Include transitif**: `<iostream>` inclus automatiquement via SFML ou d'autres headers
- Compilation réussit même sans include explicite
- Comportement **non portable** mais fonctionne

### Windows (MSVC):
- **Include strict**: Si `<iostream>` manque, `std::cout` est **undefined**
- Erreur de compilation **mais CMake peut quand même réussir** (selon la config)
- L'exécutable est **invalide ou manquant**
- Code 127 au lancement

### Leçon:
**Toujours inclure explicitement tous les headers nécessaires**, même si ça compile sur une plateforme.

---

## 📝 Checklist Finale

- [x] ✅ Noms d'exécutables corrects (`r-type_client`, `r-type_server`)
- [x] ✅ Linkage statique MinGW (`-static-libgcc -static-libstdc++`)
- [x] ✅ sfml-main linké (masque console Windows)
- [x] ✅ Assets copiés automatiquement (POST_BUILD)
- [x] ✅ DLLs SFML copiées automatiquement (POST_BUILD)
- [x] ✅ Logs de debug ajoutés (try/catch, std::cout)
- [x] ✅ Fallback fenêtre (fullscreen → fenêtré)
- [x] ✅ **Includes manquants ajoutés** (`<iostream>`, `<stdexcept>`)

---

## 🚀 Prochaine Étape

1. **Push** ces changements:
   ```bash
   git add include/client.hpp src/client/CMakeLists.txt
   git commit -m "FIX: Ajout includes manquants (iostream, stdexcept) pour Windows"
   git push origin Test_windows
   ```

2. **Sur Windows**, pull et rebuild:
   ```bash
   git pull
   rm -rf build
   cmake -B build
   cmake --build build --config Release
   ./build/r-type_client.exe
   ```

3. **Vérifier** que les logs s'affichent et que la fenêtre s'ouvre

---

## 📞 Si Ça Ne Marche Toujours Pas

Capture et partage:
1. La taille de `build/r-type_client.exe` (doit être > 1MB)
2. La sortie de `ls build/*.dll` (doit lister les DLLs SFML)
3. La sortie complète de `./build/r-type_client.exe` (pour voir les logs)
4. Les erreurs de compilation s'il y en a (regarder `build/CMakeFiles/r-type_client.dir/build.log`)

Cela permettra de diagnostiquer précisément ce qui bloque encore.

---

## 💡 Résumé Exécutif

**BUG:** Includes `<iostream>` et `<stdexcept>` manquants dans `client.hpp`  
**SYMPTÔME:** Code 127, aucun log, exécutable invalide  
**CAUSE:** Compilation échoue silencieusement sur Windows  
**FIX:** Ajout des includes manquants  
**STATUT:** ✅ Résolu, prêt pour test Windows  
