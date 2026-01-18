# 🪟 CORRECTIFS WINDOWS - R-TYPE

## Date: 18 janvier 2026

---

## ⚠️ Problèmes Identifiés

### 1. ❌ Noms d'exécutables incorrects
**Avant:** `rtype_client` et `rtype_server` (sans tiret)  
**Attendu:** `r-type_client` et `r-type_server` (avec tiret)

### 2. ❌ Client ne fonctionne pas sur Windows
**Causes:**
- Les assets sont chargés avec des chemins relatifs (`assets/`)
- Sur Windows avec CMake, les exécutables sont dans `build/Debug/` ou `build/Release/`
- Les assets sont à la racine du projet → chemins relatifs cassés
- Une console Windows s'ouvre en plus de la fenêtre du jeu

---

## ✅ Solutions Appliquées

### 1. Correction des noms d'exécutables

**Fichiers modifiés:**
- `src/client/CMakeLists.txt` : `rtype_client` → `r-type_client`
- `src/server/CMakeLists.txt` : `rtype_server` → `r-type_server`
- `.gitignore` : ajout des anciens noms pour transition

**Changements:**
```cmake
# AVANT
add_executable(rtype_client ${SRC_FILES})

# APRÈS
add_executable(r-type_client ${SRC_FILES})
```

---

### 2. Fix Windows: Copie automatique des assets

**Problème:** Sur Windows, CMake génère les exécutables dans des sous-dossiers:
```
build/
  ├─ Debug/
  │   └─ r-type_client.exe
  └─ Release/
      └─ r-type_client.exe
```

Les chemins relatifs `assets/font/...` ne fonctionnent pas.

**Solution:** Copie automatique des assets à plusieurs endroits

#### Dans `src/client/CMakeLists.txt`:
```cmake
# Copie les assets à côté de l'exécutable (Debug/Release sur Windows)
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type_client>/assets
    COMMENT "Copie du dossier assets à côté de l'exécutable (fix Windows)"
)

# Copie l'exécutable à la racine du build/ (pour lancer facilement)
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:r-type_client>
        ${CMAKE_BINARY_DIR}/r-type_client$<TARGET_FILE_SUFFIX:r-type_client>
    COMMENT "Copie de r-type_client à build/ (racine)"
)

# Copie les assets à la racine du build/ aussi
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copie des assets à build/assets/"
)
```

---

### 3. Désactivation de la console Windows

**Problème:** Sur Windows, SFML ouvre une console cmd.exe en plus de la fenêtre du jeu.

**Solution:**
```cmake
# Sur Windows, désactive la console pour une application graphique
if(WIN32)
    set_target_properties(r-type_client PROPERTIES
        WIN32_EXECUTABLE TRUE  # Pas de console cmd.exe au lancement
    )
endif()
```

---

## 📂 Structure Résultante

### Sur Linux:
```
build/
  ├─ r-type_client      ← Exécutable directement accessible
  ├─ r-type_server
  ├─ assets/            ← Copié ici
  │   ├─ font/
  │   ├─ sound/
  │   └─ music/
  └─ src/
      ├─ client/
      │   ├─ r-type_client  ← Original
      │   └─ assets/        ← Copié ici aussi
      └─ server/
          ├─ r-type_server  ← Original
          └─ assets/        ← Copié ici aussi
```

### Sur Windows:
```
build/
  ├─ r-type_client.exe  ← Copié depuis Debug/Release
  ├─ r-type_server.exe
  ├─ assets/            ← Copié ici
  │   ├─ font/
  │   ├─ sound/
  │   └─ music/
  ├─ Debug/
  │   ├─ r-type_client.exe  ← Original
  │   └─ assets/            ← Copié ici aussi
  └─ Release/
      ├─ r-type_client.exe  ← Original
      └─ assets/            ← Copié ici aussi
```

---

## 🧪 Tests de Validation

### Sur Linux:
```bash
cd build/
./r-type_server &
./r-type_client
```

### Sur Windows (PowerShell):
```powershell
cd build
.\r-type_server.exe  # Dans un terminal
.\r-type_client.exe  # Dans un autre terminal
```

Ou double-cliquer sur `r-type_client.exe` directement !

---

## 🎯 Résultats Attendus

✅ **Exécutables nommés correctement** : `r-type_client` et `r-type_server`  
✅ **Assets accessibles** depuis n'importe où on lance l'exécutable  
✅ **Pas de console Windows** pour le client (jeu graphique)  
✅ **Fonctionne sur Linux ET Windows** sans modification  

---

## 📝 Notes Techniques

### Pourquoi `$<TARGET_FILE_DIR:...>` ?

C'est une **generator expression** de CMake qui s'évalue au moment du build:
- Sur Linux : `/chemin/vers/build/src/client/`
- Sur Windows Debug : `C:\chemin\vers\build\Debug\`
- Sur Windows Release : `C:\chemin\vers\build\Release\`

Cela garantit que les assets sont copiés au bon endroit automatiquement.

### Pourquoi `$<TARGET_FILE_SUFFIX:...>` ?

Pour gérer l'extension automatiquement:
- Linux : pas d'extension → `r-type_client`
- Windows : extension `.exe` → `r-type_client.exe`

### Pourquoi copier 3 fois les assets ?

1. **À côté de l'exécutable original** (`Debug/` ou `src/client/`)
   → Pour lancer depuis le dossier de compilation
   
2. **À la racine du build/**
   → Pour lancer facilement `./build/r-type_client`
   
3. **Redondance de sécurité**
   → Si un chemin ne fonctionne pas, un autre marche

---

## 🚀 Prochaines Étapes

1. ✅ Tester sur Windows pour confirmer que tout fonctionne
2. ✅ Vérifier que les sons et musiques se chargent correctement
3. ✅ Tester la compilation Debug et Release sur Windows
4. ✅ Créer un package Windows avec installeur (optionnel)

---

## 🐛 Debugging sur Windows

Si le jeu ne démarre toujours pas sur Windows:

### 1. Vérifier les DLLs SFML
```powershell
# Dans le dossier build/Debug/ ou build/Release/
ls *.dll
```

Les DLLs SFML doivent être présentes:
- `sfml-graphics-2.dll`
- `sfml-window-2.dll`
- `sfml-system-2.dll`
- `sfml-audio-2.dll`
- `sfml-network-2.dll`
- `openal32.dll` (pour l'audio)

### 2. Lancer avec les logs
```powershell
# Lance depuis le terminal pour voir les erreurs
cd build
.\r-type_client.exe
```

Regarder les messages d'erreur comme:
```
[Resource] Erreur chargement police : assets/font/Vipnagorgialla-Rg.ttf
[Resource] Erreur chargement texture : ...
Failed to load sound: assets/sound/shoot.ogg
```

### 3. Vérifier les chemins
```powershell
# Dans build/Debug/ ou build/Release/
ls assets/font/
ls assets/sound/
ls assets/music/
```

Si les dossiers sont vides → la copie POST_BUILD n'a pas fonctionné.

---

## 📚 Ressources

- [CMake Generator Expressions](https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html)
- [SFML on Windows](https://www.sfml-dev.org/tutorials/2.6/start-vc.php)
- [WIN32_EXECUTABLE Property](https://cmake.org/cmake/help/latest/prop_tgt/WIN32_EXECUTABLE.html)
