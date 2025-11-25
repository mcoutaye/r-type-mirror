/*
** EPITECH PROJECT, 2025
** R-Type - Client test (envoi de magic "RTYP")
** Envoie la séquence magique "RTYP" en boucle, avec un délai configurable
*/
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <ip> <port> <delay_ms>\n";
        std::cout << "Ex: ./" << argv[0] << " 127.0.0.1 55002 100\n";
        return 1;
    }

    sf::IpAddress ip = argv[1];
    unsigned short port = static_cast<unsigned short>(std::stoi(argv[2]));
    int delay_ms = std::stoi(argv[3]);

    sf::UdpSocket socket;
    socket.setBlocking(false);

    // Données magiques à envoyer
    char magic[4] = {'R', 'T', 'Y', 'P'};
    std::vector<char> data(magic, magic + sizeof(magic));

    std::cout << "CLIENT TEST LANCÉ !\n";
    std::cout << "Envoi de la séquence magique \"RTYP\" toutes les " << delay_ms << "ms vers "
              << ip << ":" << port << "\n";
    std::cout << "Appuie sur Ctrl+C pour arrêter\n\n";

    int counter = 0;
    while (true) {
        if (socket.send(data.data(), data.size(), ip, port) != sf::Socket::Done) {
            std::cerr << "Erreur lors de l'envoi du paquet " << counter << std::endl;
        } else {
            counter++;
            if (counter % 100 == 0) {
                std::cout << "Envoyés : " << counter << " paquets\n";
            }
        }
        // Délai entre chaque envoi
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    return 0;
}
