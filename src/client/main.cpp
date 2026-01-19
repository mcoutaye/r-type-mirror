/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** main
*/

#include "client.hpp"
#include "texturePacker.hpp"

#include <iostream>

int main(int ac, char **av)
{
    try {
        std::cout << "[R-TYPE CLIENT] Démarrage du client..." << std::endl;
        
        sf::IpAddress ip = "127.0.0.1";
        if (ac == 2) {
            ip = sf::IpAddress(av[1]);
            std::cout << "[R-TYPE CLIENT] Connexion au serveur: " << ip << std::endl;
        } else {
            std::cout << "[R-TYPE CLIENT] Connexion au serveur: 127.0.0.1 (par défaut)" << std::endl;
        }

        std::cout << "[R-TYPE CLIENT] Initialisation du client..." << std::endl;
        Client client(ip);
        
        std::cout << "[R-TYPE CLIENT] Client initialisé avec succès!" << std::endl;
        std::cout << "[R-TYPE CLIENT] Démarrage de la boucle principale..." << std::endl;

        while (client._running) {
            client.update();       // Met à jour les inputs et la logique du jeu
            client.processInput(); // Traite les events SFML et envoie les inputs au serveur
            client.render();
        }
        
        std::cout << "[R-TYPE CLIENT] Fermeture propre du client." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "[R-TYPE CLIENT] ERREUR FATALE: " << e.what() << std::endl;
        return 84;
    } catch (...) {
        std::cerr << "[R-TYPE CLIENT] ERREUR FATALE INCONNUE" << std::endl;
        return 84;
    }
}
