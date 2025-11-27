/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Protocol
*/

#pragma once
#include <cstdint>
#include <vector>
#include <SFML/Network.hpp>

namespace Protocol {
    enum class MessageType : uint8_t {
        INPUT = 0x01,
        POSITION = 0x02,
        JOIN = 0x03,
        LEAVE = 0x04,
        NONE = 0x0,
    };

    class InputPacket {
        public:
            InputPacket() = default;
            InputPacket(uint8_t playerId, uint32_t keys);
            ~InputPacket() = default;
            std::vector<char> serialize() const;
            static InputPacket deserialize(const std::vector<char>& data);
            uint8_t getPlayerId() const;
            uint32_t getKeys() const;
        private:
            uint8_t _playerId;
            uint32_t _keys;
    };

    class PositionPacket {
        public:
            PositionPacket() = default;
            PositionPacket(uint8_t entityId, float x, float y);
            ~PositionPacket() = default;
            std::vector<char> serialize() const;
            static PositionPacket deserialize(const std::vector<char>& data);
            uint8_t getEntityId() const;
            float getX() const;
            float getY() const;
        private:
            uint8_t _entityId;
            float _x;
            float _y;
    };

    class Protocol {
        public:
            Protocol() = delete;
            static std::vector<char> createPacket(MessageType type, const std::vector<char>& data);
            static bool isValidPacket(const std::vector<char>& packet);
            static MessageType getMessageType(const std::vector<char>& packet);
            static std::vector<char> getPacketData(const std::vector<char>& packet);
            static std::vector<char> createInputPacket(const InputPacket& input);
            static std::vector<char> createPositionPacket(const PositionPacket& pos);
            static std::vector<char> createJoinPacket();
            static std::vector<char> createLeavePacket();
    };
};
