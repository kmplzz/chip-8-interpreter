#pragma once
#include <cstdint>
#include <random>

class Chip8 {
    uint8_t stack[64]{};
    uint8_t sp{};
    uint16_t pc{};
    uint16_t opcode{};
    uint8_t registers[16]{};
    uint16_t index{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t ram[4096]{};

    std::mt19937 randomGen;
    std::uniform_int_distribution<> randomDistribution;
public:
    static constexpr int SCREEN_WIDTH = 64;
    static constexpr int SCREEN_HEIGHT = 32;
    static constexpr int KEYS_COUNT = 16;

    uint8_t video[SCREEN_WIDTH][SCREEN_HEIGHT]{};
    uint8_t keyboard[KEYS_COUNT]{};

    Chip8();

    void cycle();
    uint8_t random();

    void op0nnn(); void op00E0(); void op00EE();

    void op1nnn(); void op5xy0(); void opAnnn();
    void op2nnn(); void op6xkk(); void opBnnn();
    void op3xkk(); void op7xkk(); void opCxkk();
    void op4xkk(); void op9xy0(); void opDxyn();

    void op8xy0(); void op8xy4(); void op8xyE();
    void op8xy1(); void op8xy5();
    void op8xy2(); void op8xy6();
    void op8xy3(); void op8xy7();

    void opEx9E(); void opExA1();

    void opFx07(); void opFx18(); void opFx33();
    void opFx0A(); void opFx1E(); void opFx55();
    void opFx15(); void opFx29(); void opFx65();

    static void opNOP() {};
};
