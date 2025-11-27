/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ClientPacketInterpreter
*/

#pragma once
#include "PacketInterpreter.hpp"
#include "../game/GuiLoop.hpp"

namespace Ntw {
    class ClientPacketInterpreter : public PacketInterpreter {
        public:
            ClientPacketInterpreter(UdpReceiver& receiver, GuiLoop& gameLoop);
        protected:
            void onInputPacket(const Protocol::InputPacket& input) override;
            void onPositionPacket(const Protocol::PositionPacket& pos) override;
            void onJoinPacket(const ReceivedPacket packet) override;
            void onLeavePacket(const ReceivedPacket packet) override;
        private:
            GuiLoop& _gameLoop;
    };
}
