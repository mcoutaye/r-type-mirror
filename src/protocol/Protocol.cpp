/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Protocol
*/

#include "../../include/protocol/Protocol.hpp"
#include <cstring>

Protocol::InputPacket::InputPacket(uint8_t playerId, uint32_t keys)
: _playerId(playerId), _keys(keys)
{
}

std::vector<char> Protocol::InputPacket::serialize() const
{
    std::vector<char> data(sizeof(InputPacket));
    std::memcpy(data.data(), this, sizeof(InputPacket));
    return data;
}

Protocol::InputPacket Protocol::InputPacket::deserialize(const std::vector<char>& data)
{
    InputPacket input;
    std::memcpy(&input, data.data(), sizeof(InputPacket));
    return input;
}

uint8_t Protocol::InputPacket::getPlayerId() const
{
    return _playerId;
}

uint32_t Protocol::InputPacket::getKeys() const
{
    return _keys;
}

Protocol::PositionPacket::PositionPacket(uint8_t entityId, float x, float y)
: _entityId(entityId), _x(x), _y(y)
{
}

std::vector<char> Protocol::PositionPacket::serialize() const
{
    std::vector<char> data(sizeof(PositionPacket));
    std::memcpy(data.data(), this, sizeof(PositionPacket));
    return data;
}

Protocol::PositionPacket Protocol::PositionPacket::deserialize(const std::vector<char>& data)
{
    PositionPacket pos;
    std::memcpy(&pos, data.data(), sizeof(PositionPacket));
    return pos;
}

uint8_t Protocol::PositionPacket::getEntityId() const
{
    return _entityId;
}

float Protocol::PositionPacket::getX() const
{
    return _x;
}

float Protocol::PositionPacket::getY() const
{
    return _y;
}

namespace Protocol {
    std::vector<char> Protocol::createPacket(MessageType type, const std::vector<char>& data)
    {
        std::vector<char> packet;
        packet.reserve(4 + 1 + data.size());
        packet.insert(packet.end(), {'R', 'T', 'Y', 'P'});
        packet.push_back(static_cast<char>(type));
        packet.insert(packet.end(), data.begin(), data.end());
        return packet;
    }

    bool Protocol::isValidPacket(const std::vector<char>& packet)
    {
        if (packet.size() < 5) return false;
        return (packet[0] == 'R' && packet[1] == 'T' && packet[2] == 'Y' && packet[3] == 'P');
    }

    MessageType Protocol::getMessageType(const std::vector<char>& packet)
    {
        if (!isValidPacket(packet))
            return MessageType::NONE;
        return static_cast<MessageType>(packet[4]);
    }

    std::vector<char> Protocol::getPacketData(const std::vector<char>& packet)
    {
        if (!isValidPacket(packet)) return {};
        return std::vector<char>(packet.begin() + 5, packet.end());
    }

    std::vector<char> Protocol::createInputPacket(const InputPacket& input)
    {
        return createPacket(MessageType::INPUT, input.serialize());
    }

    std::vector<char> Protocol::createPositionPacket(const PositionPacket& pos)
    {
        return createPacket(MessageType::POSITION, pos.serialize());
    }

    std::vector<char> Protocol::createJoinPacket()
    {
        return createPacket(MessageType::JOIN, {});
    }

    std::vector<char> Protocol::createLeavePacket()
    {
        return createPacket(MessageType::LEAVE, {});
    }
}
