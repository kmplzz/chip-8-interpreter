#pragma once

#include <cstdint>
#include <SFML/Graphics.hpp>
#include "Chip8.h"

class Renderer {
    uint8_t (&m_video)[Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT];
    uint8_t (&m_keyboard)[Chip8::KEYS_COUNT];

    sf::RenderWindow* m_window;
    sf::RenderTexture* m_texture;
    sf::Sprite* m_sprite;
    sf::Event m_event{};

public:
    Renderer(uint8_t (&video)[Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT],
        uint8_t (&keyboard)[Chip8::KEYS_COUNT], int scale);
    ~Renderer();

    bool processInput();
    void update() const;
};
