#pragma once
#include <cstdint>

class Chip8 {
public:
    static constexpr int SCREEN_WIDTH = 64;
    static constexpr int SCREEN_HEIGHT = 32;
    static constexpr int KEYS_COUNT = 16;

    uint8_t video[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t keyboard[KEYS_COUNT];

    Chip8();

    void cycle();
};
