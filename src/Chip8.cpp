#include "Chip8.h"

#include <cstring>
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
}

void Chip8::cycle() {
}

uint8_t Chip8::random() {
    return randomDistribution(randomGen);
}

void Chip8::op0nnn() {
}

void Chip8::op00E0() {
    memset(video, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void Chip8::op00EE() {
    pc = stack[sp--];
}

void Chip8::op1nnn() {
    pc = opcode & 0x0FFF;
}

void Chip8::op2nnn() {
    stack[++sp] = pc;
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
    registers[vx] = opcode & 0x00FF;
}

void Chip8::op7xkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[vx] += opcode & 0x00FF;
}

void Chip8::opAnnn() {
    index = opcode & 0x0FFF;
}

void Chip8::opBnnn() {
    pc = (opcode & 0x0FFF) + registers[0];
}

void Chip8::opCxkk() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t value = opcode & 0xFF;
    registers[vx] = random() & value;
}

void Chip8::opDxyn() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    const uint8_t n = opcode & 0xF;
    const uint8_t x = registers[vx];
    uint8_t y = registers[vy];
    for (int i = index; i < index + n; i++) {
        y %= SCREEN_HEIGHT;
        const uint8_t row = ram[i];
        for (int j = 7; j >= 0; j--) {
            const uint8_t next = (row >> j) & 0x1;
            const uint8_t idx = x + 7 - j;
            const uint8_t current = video[idx][y];
            const uint8_t value = current ^ next;
            registers[0xF] = value;
            video[idx][y] = value;
        }
        y++;
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
    registers[vx] = registers[vx] | registers[vy];
}

void Chip8::op8xy2() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] = registers[vx] & registers[vy];
}

void Chip8::op8xy3() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[vx] = registers[vx] ^ registers[vy];
}

void Chip8::op8xy4() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    const uint16_t sum = vx + vy;
    registers[0xF] = sum & 0xFF00 ? 1 : 0;
    registers[vx] = sum & 0xFF;
}

void Chip8::op8xy5() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[0xF] = vx >= vy ? 0 : 1;
    registers[vx] = (vx - vy) & 0x00FF;
}

void Chip8::op8xy6() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = vx & 0x1;
    registers[vx] >>= 1;
}

void Chip8::op8xy7() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    const uint8_t vy = (opcode & 0x00F0) >> 4;
    registers[0xF] = vy >= vx ? 0 : 1;
    registers[vx] = (vy - vx) & 0x00FF;
}

void Chip8::op8xyE() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = vx >> 7 ? 1 : 0;
    registers[vx] <<= 2;
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
    ram[index] = vx / 100;
    ram[index + 1] = vx % 100 / 10;
    ram[index + 2] = vx % 100 % 10;
}

void Chip8::opFx55() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= vx; i++)
        ram[index + i] = registers[i];
    index += vx + 1;
}

void Chip8::opFx65() {
    const uint8_t vx = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= vx; i++)
        registers[i] = ram[index + i];
    index += vx + 1;
}
