/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "R-Type Game Engine", "index.html", [
    [ "Documentation de la Game Engine dans le Projet R-Type", "index.html#autotoc_md0", [
      [ "1. Introduction à la Game Engine", "index.html#autotoc_md1", [
        [ "Relations clés :", "index.html#autotoc_md2", null ]
      ] ],
      [ "2. L'Entity Component System (ECS)", "index.html#autotoc_md3", [
        [ "Concepts Clés de l'ECS :", "index.html#autotoc_md4", null ],
        [ "Relations dans l'ECS :", "index.html#autotoc_md5", null ],
        [ "Exemple de flux :", "index.html#autotoc_md6", null ]
      ] ],
      [ "3. Les Composants", "index.html#autotoc_md7", [
        [ "Exemples principaux :", "index.html#autotoc_md8", null ],
        [ "Relations :", "index.html#autotoc_md9", null ]
      ] ],
      [ "4. Les Systèmes", "index.html#autotoc_md10", [
        [ "Liste des systèmes (dans <tt>systems/</tt> ) :", "index.html#autotoc_md11", null ],
        [ "Autres :", "index.html#autotoc_md12", null ],
        [ "Relations générales des systèmes :", "index.html#autotoc_md13", null ]
      ] ],
      [ "5. Utilisation dans le <tt>main.cpp</tt> (Client)", "index.html#autotoc_md14", [
        [ "Initialisation :", "index.html#autotoc_md15", null ],
        [ "Setup du Stage :", "index.html#autotoc_md16", null ],
        [ "Boucle de Jeu :", "index.html#autotoc_md17", null ],
        [ "Relations dans <tt>main</tt> :", "index.html#autotoc_md18", null ]
      ] ],
      [ "6. Relations Globales et Flux", "index.html#autotoc_md19", null ]
    ] ],
    [ "Bienvenue dans la Documentation de R-Type", "index.html#autotoc_md20", [
      [ "Aperçu Général", "index.html#autotoc_md21", null ],
      [ "Sections Principales", "index.html#autotoc_md22", null ],
      [ "Comment Naviguer", "index.html#autotoc_md23", null ],
      [ "Contact", "index.html#autotoc_md24", null ]
    ] ],
    [ "Proof of Concept: Choice of SFML for R-Type Project", "md_poc.html", [
      [ "1. Introduction", "md_poc.html#autotoc_md26", null ],
      [ "2. Comparison with Alternatives", "md_poc.html#autotoc_md27", [
        [ "A. SFML vs. SDL2 (Simple DirectMedia Layer)", "md_poc.html#autotoc_md28", null ],
        [ "B. SFML vs. Raylib", "md_poc.html#autotoc_md29", null ],
        [ "C. SFML vs. Qt (Qt 6)", "md_poc.html#autotoc_md30", null ]
      ] ],
      [ "3. Why SFML is the Superior Choice for R-Type", "md_poc.html#autotoc_md31", [
        [ "A. Unified Networking & Graphics", "md_poc.html#autotoc_md32", null ],
        [ "B. Protocol & Serialization (<tt>sf::Packet</tt>)", "md_poc.html#autotoc_md33", null ],
        [ "C. Modern C++ Compatibility", "md_poc.html#autotoc_md34", null ],
        [ "D. Development Velocity", "md_poc.html#autotoc_md35", null ]
      ] ],
      [ "4. Architectural Study: ECS vs. OOP", "md_poc.html#autotoc_md36", [
        [ "A. Traditional OOP (Object-Oriented Programming)", "md_poc.html#autotoc_md37", null ],
        [ "B. ECS (Entity Component System) - Chosen for R-Type", "md_poc.html#autotoc_md38", null ]
      ] ],
      [ "5. Algorithms & Data Structures", "md_poc.html#autotoc_md39", [
        [ "A. Algorithms", "md_poc.html#autotoc_md40", null ],
        [ "B. Data Structures", "md_poc.html#autotoc_md41", null ]
      ] ],
      [ "6. Network Protocol & Reliability", "md_poc.html#autotoc_md42", [
        [ "A. Hybrid Protocol Strategy", "md_poc.html#autotoc_md43", null ],
        [ "B. Protocol Documentation (RFC Style)", "md_poc.html#autotoc_md44", null ]
      ] ],
      [ "7. Security & Data Integrity", "md_poc.html#autotoc_md45", [
        [ "A. Vulnerabilities", "md_poc.html#autotoc_md46", null ],
        [ "B. Mitigation & Monitoring", "md_poc.html#autotoc_md47", null ]
      ] ],
      [ "8. Documentation Strategy", "md_poc.html#autotoc_md48", null ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Enumerations", "functions_enum.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"CollisionSystem_8hpp.html",
"classUDP.html#ae94f4f5c790cc0913de9ff26821e8842",
"structComponents_1_1MovementPattern.html#a6e71a27920ec592bfec9e63b6970e4fe"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';