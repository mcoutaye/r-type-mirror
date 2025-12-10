/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** PacketInterpreter
*/

#include "PacketInterpreter.hpp"
#include <iostream>
#include <iomanip>

Ntw::PacketInterpreter::PacketInterpreter(UdpReceiver& receiver)
: _receiver(receiver)
{
}

Ntw::PacketInterpreter::~PacketInterpreter()
{
    stop();
}

void Ntw::PacketInterpreter::start()
{
    if (!_running) {
        _running = true;
        _thread = std::thread(&PacketInterpreter::interpreterLoop, this);
    }
}

void Ntw::PacketInterpreter::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void Ntw::PacketInterpreter::join()
{
    if (_thread.joinable())
        _thread.join();
}

void Ntw::PacketInterpreter::interpreterLoop()
{
    ReceivedPacket packet;

    while (_running) {
        // 1. Récupère tous les paquets arrivés
        while (_receiver.getPacket(packet)) {
            std::lock_guard<std::mutex> lock(_mutex);
            _packetBuffer.push(std::move(packet)); // ← std::move pour éviter les copies
        }

        // 2. Traite tous les paquets en attente
        while (!_packetBuffer.empty()) {
            std::lock_guard<std::mutex> lock(_mutex);
            packet = std::move(_packetBuffer.front());
            _packetBuffer.pop();

            if (packet._data.size() < 4) {
                std::cout << "[Interpreter] Paquet trop court (" 
                          << packet._data.size() << " bytes) de " 
                          << packet._sender << ":" << packet._port << std::endl;
                continue;
            }

            // Les 4 premiers octets = le magic
            const char* magic = packet._data.data();

            // Vérification du magic RTYP
            if (magic[0] == 'R' && magic[1] == 'T' && magic[2] == 'Y' && magic[3] == 'P') {
                std::cout << "[Interpreter] Magic RTYP reçu ! (valide) de "
                          << packet._sender << ":" << packet._port
                          << " | taille totale: " << packet._data.size() << " bytes" << std::endl;
            } else {
                // Affichage en hexa si ce n'est pas RTYP → tu vois exactement ce qui arrive
                std::cout << "[Interpreter] Magic invalide, reçu : ";
                for (int i = 0; i < 4; ++i) {
                    std::cout << "0x" << std::hex << std::uppercase 
                              << (static_cast<unsigned int>(static_cast<uint8_t>(magic[i]))) << " ";
                }
                std::cout << std::dec << "(soit \"";
                for (int i = 0; i < 4; ++i)
                    std::cout << (std::isprint(magic[i]) ? magic[i] : '.');
                std::cout << "\") de " << packet._sender << ":" << packet._port << std::endl;
            }

            // Optionnel : affiche les 16 premiers octets en hexa (très utile)
            if (packet._data.size() >= 8) {
                std::cout << "   Premier payload (hex) : ";
                for (size_t i = 4; i < std::min(packet._data.size(), size_t(20)); ++i) {
                    std::cout << std::hex << std::uppercase
                              << std::setw(2) << std::setfill('0')
                              << (static_cast<unsigned int>(static_cast<uint8_t>(packet._data[i]))) << " ";
                }
                std::cout << std::dec << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
