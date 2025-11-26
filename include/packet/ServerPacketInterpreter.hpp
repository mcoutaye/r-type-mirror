/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ServerPacketInterpreter
*/

#pragma once
#include "PacketInterpreter.hpp"

namespace Ntw {
    class ServerPacketInterpreter : public PacketInterpreter {
        public:
            ServerPacketInterpreter(UdpReceiver& receiver);
            ~ServerPacketInterpreter() = default;
        protected:
            void onInputPacket(const Protocol::InputPacket& input) override;
            void onPositionPacket(const Protocol::PositionPacket& pos) override;
    };
}
