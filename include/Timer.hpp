/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Timer
*/

#ifndef TIMER_HPP_

    #define TIMER_HPP_
    #include <SFML/System.hpp>

class Timer {
    public:
        explicit Timer() {
            _clock.restart();
        }
        ~Timer() {}

        sf::Int64 getLastTick() {
            return _lastTick.asMicroseconds();
        }

        uint16_t getCurrentFrame() {
            return _frameNumber;
        }

        void updateClock() {
            auto elapsedTime = _clock.getElapsedTime();
            if (elapsedTime.asMicroseconds() > 60000) {
                _lastTick = elapsedTime;
                _frameNumber++;
                _clock.restart();
            }
        }
    private:
        sf::Clock _clock;
        sf::Time _lastTick;
        uint16_t _frameNumber;
};

#endif /* !TIMER_HPP_ */
