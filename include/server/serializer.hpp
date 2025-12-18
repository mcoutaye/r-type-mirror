/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** serializer
*/

#ifndef SERIALIZER_HPP_
    #define SERIALIZER_HPP_

    #include <vector>
    #include <cstring>
    #include <cstdint>

class Serializer {
public:
    // Converts any POD struct to a vector of bytes
    template <typename T>
    static std::vector<std::uint8_t> serialize(const T& data) {
        // Ensure T is safe to copy (no std::string, std::vector inside)
        static_assert(std::is_trivially_copyable<T>::value, "Type must be POD to be serialized directly");

        std::vector<std::uint8_t> buffer(sizeof(T));
        std::memcpy(buffer.data(), &data, sizeof(T));
        return buffer;
    }

    // Converts bytes back to a struct
    template <typename T>
    static T deserialize(const std::vector<std::uint8_t>& data) {
        T instance;
        if (data.size() < sizeof(T)) return instance; // Error handling
        std::memcpy(&instance, data.data(), sizeof(T));
        return instance;
    }
};

#endif /* !SERIALIZER_HPP_ */
