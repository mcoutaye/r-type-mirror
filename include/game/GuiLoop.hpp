/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** GuiLoop
*/

#pragma once
#include "GameLoop.hpp"

class GuiLoop : public GameLoop {
    public:
        GuiLoop(Ntw::UdpSender& sender);
    protected:
        void gameLoop() override;
};
