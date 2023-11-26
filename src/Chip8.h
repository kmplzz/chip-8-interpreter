#pragma once
#include <array>
#include <random>

using namespace std;

class Chip8 {

    static constexpr int RAM_SIZE = 4096;
    static constexpr int STACK_SIZE = 8;
    static constexpr int REGISTERS_COUNT = 16;

    array<uint8_t, RAM_SIZE> ram{};

    uint16_t pc;
    uint16_t opcode{};

    uint8_t sp{};
    array<uint16_t, STACK_SIZE> stack{};

    uint16_t index{};
    std::array<uint8_t, REGISTERS_COUNT> registers{};

    uint8_t delayTimer{};
    uint8_t soundTimer{};

    typedef void (Chip8::*Ops)();
    array<Ops, 0xF + 1> ops{};
    array<Ops, 0xF + 1> ops0{};
    array<Ops, 0xF + 1> ops8{};
    array<Ops, 0xF + 1> opsE{};
    array<Ops, 0x65 + 1> opsF{};

    void op00E0(); void op00EE();

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

    void opNOP();

    [[nodiscard]] uint8_t getVx() const;
    [[nodiscard]] uint8_t getVy() const;

    mt19937 randomGen;
    uniform_int_distribution<> randomDistribution;
    uint8_t random();
public:
    static constexpr int SCREEN_WIDTH = 64;
    static constexpr int SCREEN_HEIGHT = 32;
    static constexpr int KEYS_COUNT = 16;

    std::array<std::array<uint8_t, SCREEN_HEIGHT>, SCREEN_WIDTH> framebuffer{};
    std::array<uint8_t, KEYS_COUNT> keyboard{};

    Chip8();

    void load(const string &filename);

    void cycle();
};
