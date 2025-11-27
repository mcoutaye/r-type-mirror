/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** ServerPacketInterpreter
*/

#pragma once
#include "PacketInterpreter.hpp"
#include "../network/NetworkManager.hpp"
#include "../network/UdpReceiver.hpp"
#include "../game/GamePlayLoop.hpp"

namespace Ntw {
    class ServerPacketInterpreter : public PacketInterpreter {
        public:
            ServerPacketInterpreter(UdpReceiver& receiver, Ntw::NetworkManager& networkManager, GamePlayLoop& gameLoop);
        protected:
            void onInputPacket(const Protocol::InputPacket& input) override;
            void onPositionPacket(const Protocol::PositionPacket& pos) override;
            void onJoinPacket(const ReceivedPacket packet) override;
            void onLeavePacket(const ReceivedPacket packet) override;
        private:
            ReceivedPacket packet;
            Ntw::NetworkManager& _networkManager;
            GamePlayLoop& _gameLoop;
    };
}
