#pragma once

#include <cstdint>
#include <SFML/Graphics.hpp>
#include "Chip8.h"

class Renderer {
    uint8_t (&m_framebuffer)[Chip8::SCREEN_WIDTH][Chip8::SCREEN_HEIGHT];
    uint8_t (&m_keyboard)[Chip8::KEYS_COUNT];

    sf::RenderWindow* m_window;
    sf::RenderTexture* m_texture;
    sf::Sprite* m_sprite;
    sf::Event m_event{};

    using Key = sf::Keyboard::Key;
    std::map<Key, uint8_t> keyCodes = {
        {Key::Num1, 0x1}, {Key::Num2, 0x2}, {Key::Num3, 0x3}, {Key::Num4, 0xC},
        {Key::Q, 0x4}, {Key::W, 0x5}, {Key::E, 0x6}, {Key::R, 0xD},
        {Key::A, 0x7}, {Key::S, 0x8}, {Key::D, 0x9}, {Key::F, 0xE},
        {Key::Z, 0xA}, {Key::X, 0x0}, {Key::C, 0xB}, {Key::V, 0xF},
    };
public:
    Renderer(uint8_t (&framebuffer)[Chip8::SCREEN_WIDTH][Chip8::SCREEN_HEIGHT],
        uint8_t (&keyboard)[Chip8::KEYS_COUNT], int scale);
    ~Renderer();

    bool processInput();
    void update() const;
};
