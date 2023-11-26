#include "Chip8.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <random>

constexpr uint8_t FONT_SPRITE_HEIGHT = 5;
constexpr uint8_t FONT_SET[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

Chip8::Chip8() {
    std::random_device rd;
    randomGen = std::mt19937(rd());
    randomDistribution = std::uniform_int_distribution(0, 255);

    memcpy(ram, FONT_SET, sizeof(FONT_SET));

    for (int i = 0; i < 16; i++)
        ops[i] = ops0[i] = ops8[i] = opsE[i] = &Chip8::opNOP;

    for (auto & i : opsF)
        i = &Chip8::opNOP;

    ops[0x1] = &Chip8::op1nnn; ops[0x2] = &Chip8::op2nnn; ops[0x3] = &Chip8::op3xkk; ops[0x4] = &Chip8::op4xkk;
    ops[0x5] = &Chip8::op5xy0; ops[0x6] = &Chip8::op6xkk; ops[0x7] = &Chip8::op7xkk; ops[0x9] = &Chip8::op9xy0;
    ops[0xA] = &Chip8::opAnnn; ops[0xB] = &Chip8::opBnnn; ops[0xC] = &Chip8::opCxkk; ops[0xD] = &Chip8::opDxyn;

    ops0[0x0] = &Chip8::op00E0; ops0[0xE] = &Chip8::op00EE;

    ops8[0x0] = &Chip8::op8xy0; ops8[0x1] = &Chip8::op8xy1; ops8[0x2] = &Chip8::op8xy2; ops8[0x3] = &Chip8::op8xy3;
    ops8[0x4] = &Chip8::op8xy4; ops8[0x5] = &Chip8::op8xy5; ops8[0x6] = &Chip8::op8xy6; ops8[0x7] = &Chip8::op8xy7;
    ops8[0xE] = &Chip8::op8xyE;

    opsE[0x9] = &Chip8::opEx9E; opsE[0xA] = &Chip8::opExA1;

    opsF[0x07] = &Chip8::opFx07; opsF[0x0A] = &Chip8::opFx0A; opsF[0x15] = &Chip8::opFx15; opsF[0x18] = &Chip8::opFx18;
    opsF[0x1E] = &Chip8::opFx1E; opsF[0x29] = &Chip8::opFx29; opsF[0x33] = &Chip8::opFx33; opsF[0x55] = &Chip8::opFx55;
    opsF[0x65] = &Chip8::opFx65;

    pc = 0x200;
}

void Chip8::cycle() {
    opcode = (ram[pc] << 8) | ram[pc + 1];
    pc += 2;
    uint8_t idx;
    switch (const uint8_t op = opcode >> 12) {
        case 0xF:
            idx = opcode & 0xFF;
            (this->*opsF[idx])();
            break;
        case 0xE:
            idx = (opcode >> 4) & 0xF;
            (this->*opsE[idx])();
            break;
        case 0x8:
            idx = opcode & 0xF;
            (this->*ops8[idx])();
            break;
        case 0x0:
            idx = (opcode >> 4) & 0xF;
            (this->*ops0[idx])();
            break;
        default:
            (this->*ops[op])();
            break;
    }
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

void Chip8::loadRom(const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        const std::streampos size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        for (long i = 0; i < size; ++i) {
            ram[0x200 + i] = buffer[i];
        }
        delete[] buffer;
    }
}

uint8_t Chip8::random() {
    return randomDistribution(randomGen);
}

void Chip8::op00E0() {
    for (auto & i : framebuffer)
        memset(i, 0, SCREEN_HEIGHT);
}

void Chip8::op00EE() {
    sp--;
    pc = stack[sp];
}

void Chip8::op1nnn() {
    pc = (opcode & 0x0FFF);
}

void Chip8::op2nnn() {
    stack[sp] = pc;
    sp++;
    pc = opcode & 0xFFF;
}

void Chip8::op3xkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t value = opcode & 0xFF;
    if (registers[vx] == value) pc += 2;
}

void Chip8::op4xkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t value = opcode & 0xFF;
    if (registers[vx] != value) pc += 2;
}

void Chip8::op5xy0() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    if (registers[vx] == registers[vy]) pc += 2;
}

void Chip8::op6xkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[vx] = (opcode & 0x00FF);
}

void Chip8::op7xkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[vx] += (opcode & 0x00FF);
}

void Chip8::opAnnn() {
    index = opcode & 0x0FFF;
}

void Chip8::opBnnn() {
    pc = registers[0] + (opcode & 0x0FFF);
}

void Chip8::opCxkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t value = opcode & 0xFF;

    registers[vx] = random() & value;
    std::map<int, int> map;
}

void Chip8::opDxyn() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    const uint8_t n = opcode & 0xF;
    const uint8_t x = registers[vx] % SCREEN_WIDTH;
    const uint8_t y = registers[vy] % SCREEN_HEIGHT;
    registers[0xF] = 0;
    for (int i = 0; i < n; i++) {
        const uint8_t row = ram[index + i];
        for (int j = 7; j >= 0; j--) {
            const uint8_t next = (row >> j) & 0x1;
            const uint8_t idx = x + 7 - j;
            const uint8_t current = framebuffer[idx][y + i];
            if (next) {
                if (current) {
                    registers[0xF] = 1;
                }
                framebuffer[idx][y + i] = (current ^ 0xF);
            }
        }
    }
}

void Chip8::op8xy0() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] = registers[vy];
}

void Chip8::op8xy1() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] |= registers[vy];
}

void Chip8::op8xy2() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] &= registers[vy];
}

void Chip8::op8xy3() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] ^= registers[vy];
}

void Chip8::op8xy4() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    const uint16_t sum = registers[vx] + registers[vy];
    registers[0xF] = (sum > 0xFF) ? 1 : 0;
    registers[vx] = sum & 0xFF;
}

void Chip8::op8xy5() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[0xF] = registers[vx] > registers[vy] ? 1 : 0;
    registers[vx] -= registers[vy];
}

void Chip8::op8xy6() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = registers[vx] & 0x1;
    registers[vx] >>= 1;
}

void Chip8::op8xy7() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[0xF] = registers[vy] > registers[vx] ? 1 : 0;
    registers[vx] = registers[vy] - registers[vx];
}

void Chip8::op8xyE() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = (registers[vx] & 0x80) ? 1 : 0;
    registers[vx] <<= 1;
}

void Chip8::op9xy0() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    if (registers[vx] != registers[vy]) pc += 2;
}

void Chip8::opEx9E() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    if (keyboard[registers[vx]]) pc += 2;
}

void Chip8::opExA1() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    if (!keyboard[registers[vx]]) pc += 2;
}

void Chip8::opFx07() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[vx] = delayTimer;
}

void Chip8::opFx0A() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    for (int i = 0; i < KEYS_COUNT; i++) {
        if (keyboard[i]) {
            registers[vx] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::opFx15() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    delayTimer = registers[vx];
}

void Chip8::opFx18() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    soundTimer = registers[vx];
}

void Chip8::opFx1E() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    index += registers[vx];
}

void Chip8::opFx29() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    index = registers[vx] * FONT_SPRITE_HEIGHT;
}

void Chip8::opFx33() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[vx];
    ram[index + 2] = value % 10;
    value /= 10;
    ram[index + 1] = value % 10;
    value /= 10;
    ram[index] = value % 10;
}

void Chip8::opFx55() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= vx; i++)
        ram[index + i] = registers[i];
}

void Chip8::opFx65() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= vx; i++)
        registers[i] = ram[index + i];
}

void Chip8::opNOP() {
}
