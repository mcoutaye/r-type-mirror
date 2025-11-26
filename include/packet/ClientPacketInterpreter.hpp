/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ClientPacketInterpreter
*/

#pragma once
#include "PacketInterpreter.hpp"

namespace Ntw {
    class ClientPacketInterpreter : public PacketInterpreter {
        public:
            ClientPacketInterpreter(UdpReceiver& receiver);
            ~ClientPacketInterpreter() = default;
        protected:
            void onInputPacket(const Protocol::InputPacket& input) override;
            void onPositionPacket(const Protocol::PositionPacket& pos) override;
    };
}
