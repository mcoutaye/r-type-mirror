# 📚 Documentation du Projet R-Type

Ce dossier contient la documentation générée du projet.

## Structure

- `engine.html` - Documentation du moteur de jeu (générée depuis ENGINE_DOCUMENTATION.md)
- `doxygen/` - Documentation API générée par Doxygen
- `index.html` - Page d'accueil de la documentation

## Utilisation

Pour afficher la documentation:

```bash
# Depuis la racine du projet
make docs

# Ou avec CMake directement
cmake --build build --target docs
```

La documentation sera accessible sur http://localhost:8080
