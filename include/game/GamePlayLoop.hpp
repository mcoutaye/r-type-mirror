/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GamePlayLoop
*/

#pragma once
#include "GameLoop.hpp"
#include "../network/NetworkManager.hpp"

class GamePlayLoop : public GameLoop {
    public:
        GamePlayLoop(Ntw::UdpSender& sender, Ntw::NetworkManager& networkManager);
    protected:
        void gameLoop() override;
    private:
        Ntw::NetworkManager& _networkManager;
};
