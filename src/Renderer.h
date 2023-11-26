#pragma once

#include <SFML/Graphics.hpp>
#include "Chip8.h"

using namespace sf;

class Renderer {

    Color bgColor;
    Color fgColor;

    RenderWindow* window;
    RenderTexture* texture;
    Sprite* sprite;
    Event event{};

    using Key = Keyboard::Key;
    std::map<Key, uint8_t> keyCodes = {
        {Key::Num1, 0x1}, {Key::Num2, 0x2}, {Key::Num3, 0x3}, {Key::Num4, 0xC},
        {Key::Q, 0x4}, {Key::W, 0x5}, {Key::E, 0x6}, {Key::R, 0xD},
        {Key::A, 0x7}, {Key::S, 0x8}, {Key::D, 0x9}, {Key::F, 0xE},
        {Key::Z, 0xA}, {Key::X, 0x0}, {Key::C, 0xB}, {Key::V, 0xF},
    };
public:
    Renderer(int width, int height, int scale, Color bgColor, Color fgColor);

    ~Renderer();

    bool input(std::array<uint8_t, Chip8::KEYS_COUNT> &keyboard);

    void update(const std::array<std::array<uint8_t, Chip8::SCREEN_HEIGHT>, Chip8::SCREEN_WIDTH> &framebuffer) const;
};
