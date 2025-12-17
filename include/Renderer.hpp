/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Renderer
*/

#ifndef RENDERER_HPP_
    #define RENDERER_HPP_

    #include <SFML/Graphics.hpp>
    #include "ecs.hpp"

class Renderer {
    public:
        Renderer(ECS &ecs);
        ~Renderer();

    private:
        ECS &_ecs;

        sf::RenderWindow &_window;
};

Renderer::Renderer(ECS &ecs)
    : _ecs(ecs)
{
    _window.create(sf::VideoMode(1920, 1080), "R-TYPE - CLIENT", sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
}

Renderer::~Renderer() {}



#endif /* !RENDERER_HPP_ */
