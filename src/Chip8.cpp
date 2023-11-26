#include "Chip8.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <random>

constexpr uint16_t PC_START_ADDRESS = 0x200;
constexpr uint8_t FLAG_REGISTER = 0xF;
constexpr uint8_t FONT_SPRITE_HEIGHT = 5;
constexpr auto FONT_SET = std::to_array<uint8_t>({
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
});

Chip8::Chip8() {
    random_device rd;
    randomGen = std::mt19937(rd());
    randomDistribution = std::uniform_int_distribution(0, 255);

    copy_n(std::begin(FONT_SET), FONT_SET.size(), begin(ram));

    ranges::fill(ops, &Chip8::opNOP);
    ops[0x1] = &Chip8::op1nnn; ops[0x2] = &Chip8::op2nnn; ops[0x3] = &Chip8::op3xkk; ops[0x4] = &Chip8::op4xkk;
    ops[0x5] = &Chip8::op5xy0; ops[0x6] = &Chip8::op6xkk; ops[0x7] = &Chip8::op7xkk; ops[0x9] = &Chip8::op9xy0;
    ops[0xA] = &Chip8::opAnnn; ops[0xB] = &Chip8::opBnnn; ops[0xC] = &Chip8::opCxkk; ops[0xD] = &Chip8::opDxyn;

    ranges::fill(ops0, &Chip8::opNOP);
    ops0[0x0] = &Chip8::op00E0; ops0[0xE] = &Chip8::op00EE;

    ranges::fill(ops8, &Chip8::opNOP);
    ops8[0x0] = &Chip8::op8xy0; ops8[0x1] = &Chip8::op8xy1; ops8[0x2] = &Chip8::op8xy2; ops8[0x3] = &Chip8::op8xy3;
    ops8[0x4] = &Chip8::op8xy4; ops8[0x5] = &Chip8::op8xy5; ops8[0x6] = &Chip8::op8xy6; ops8[0x7] = &Chip8::op8xy7;
    ops8[0xE] = &Chip8::op8xyE;

    ranges::fill(opsE, &Chip8::opNOP);
    opsE[0x9] = &Chip8::opEx9E; opsE[0xA] = &Chip8::opExA1;

    ranges::fill(opsF, &Chip8::opNOP);
    opsF[0x07] = &Chip8::opFx07; opsF[0x0A] = &Chip8::opFx0A; opsF[0x15] = &Chip8::opFx15; opsF[0x18] = &Chip8::opFx18;
    opsF[0x1E] = &Chip8::opFx1E; opsF[0x29] = &Chip8::opFx29; opsF[0x33] = &Chip8::opFx33; opsF[0x55] = &Chip8::opFx55;
    opsF[0x65] = &Chip8::opFx65;

    pc = PC_START_ADDRESS;
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

void Chip8::load(const std::string &filename) {
    if (ifstream file(filename, ios::binary | ios::ate); file.is_open()) {
        const streampos size = file.tellg();
        char* buffer = new char[size];
        file.seekg(0, ios::beg);
        file.read(buffer, size);
        file.close();
        for (long i = 0; i < size; ++i) {
            ram[PC_START_ADDRESS + i] = buffer[i];
        }
        delete[] buffer;
    }
}

uint8_t Chip8::random() {
    return randomDistribution(randomGen);
}

void Chip8::op00E0() {
    for (auto i : framebuffer)
        std::ranges::fill(i, 0);
}

void Chip8::op00EE() {
    pc = stack[--sp];
}

void Chip8::op1nnn() {
    pc = (opcode & 0x0FFF);
}

void Chip8::op2nnn() {
    stack[sp++] = pc;
    pc = opcode & 0xFFF;
}

void Chip8::op3xkk() {
    const uint8_t value = opcode & 0xFF;
    if (registers[getVx()] == value) pc += 2;
}

void Chip8::op4xkk() {
    const uint8_t value = opcode & 0xFF;
    if (registers[getVx()] != value) pc += 2;
}

void Chip8::op5xy0() {
    if (registers[getVx()] == registers[getVy()]) pc += 2;
}

void Chip8::op6xkk() {
    registers[getVx()] = (opcode & 0x00FF);
}

void Chip8::op7xkk() {
    registers[getVx()] += (opcode & 0x00FF);
}

void Chip8::opAnnn() {
    index = opcode & 0x0FFF;
}

void Chip8::opBnnn() {
    pc = registers[0] + (opcode & 0x0FFF);
}

void Chip8::opCxkk() {
    const uint8_t value = opcode & 0xFF;
    registers[getVx()] = random() & value;
}

void Chip8::opDxyn() {
    const uint8_t spriteHeight = opcode & 0xF;
    const uint8_t x = registers[getVx()] % SCREEN_WIDTH;
    const uint8_t y = registers[getVy()] % SCREEN_HEIGHT;
    registers[FLAG_REGISTER] = 0;
    for (int row = 0; row < spriteHeight; row++) {
        const uint8_t spriteByte = ram[index + row];
        for (int col = 7; col >= 0; col--) {
            const uint8_t idx = x + 7 - col;
            const uint8_t spritePixel = (spriteByte >> col) & 0x1;
            const uint8_t screenPixel = framebuffer[idx][y + row];
            const uint8_t pixel = spritePixel ^ screenPixel;
            framebuffer[idx][y + row] = pixel;
            if (!pixel) registers[FLAG_REGISTER] = 1;
        }
    }
}

void Chip8::op8xy0() {
    registers[getVx()] = registers[getVy()];
}

void Chip8::op8xy1() {
    registers[getVx()] |= registers[getVy()];
}

void Chip8::op8xy2() {
    registers[getVx()] &= registers[getVy()];
}

void Chip8::op8xy3() {
    registers[getVx()] ^= registers[getVy()];
}

void Chip8::op8xy4() {
    const uint8_t vx = getVx();
    const uint16_t sum = registers[vx] + registers[getVy()];
    registers[FLAG_REGISTER] = (sum > 0xFF) ? 1 : 0;
    registers[vx] = sum & 0xFF;
}

void Chip8::op8xy5() {
    const uint8_t vx = getVx();
    const uint8_t vy = getVy();
    registers[FLAG_REGISTER] = registers[vx] > registers[vy] ? 1 : 0;
    registers[vx] -= registers[vy];
}

void Chip8::op8xy6() {
    const uint8_t vx = getVx();
    registers[FLAG_REGISTER] = registers[vx] & 0x1;
    registers[vx] >>= 1;
}

void Chip8::op8xy7() {
    const uint8_t vx = getVx();
    const uint8_t vy = getVy();
    registers[FLAG_REGISTER] = registers[vy] > registers[vx] ? 1 : 0;
    registers[vx] = registers[vy] - registers[vx];
}

void Chip8::op8xyE() {
    const uint8_t vx = getVx();
    registers[FLAG_REGISTER] = (registers[vx] & 0x80) ? 1 : 0;
    registers[vx] <<= 1;
}

void Chip8::op9xy0() {
    if (registers[getVx()] != registers[getVy()]) pc += 2;
}

void Chip8::opEx9E() {
    if (keyboard[registers[getVx()]]) pc += 2;
}

void Chip8::opExA1() {
    if (!keyboard[registers[getVx()]]) pc += 2;
}

void Chip8::opFx07() {
    registers[getVx()] = delayTimer;
}

void Chip8::opFx0A() {
    for (int i = 0; i < KEYS_COUNT; i++) {
        if (keyboard[i]) {
            registers[getVx()] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::opFx15() {
    delayTimer = registers[getVx()];
}

void Chip8::opFx18() {
    soundTimer = registers[getVx()];
}

void Chip8::opFx1E() {
    index += registers[getVx()];
}

void Chip8::opFx29() {
    index = registers[getVx()] * FONT_SPRITE_HEIGHT;
}

void Chip8::opFx33() {
    uint8_t value = registers[getVx()];
    ram[index + 2] = value % 10;
    value /= 10;
    ram[index + 1] = value % 10;
    value /= 10;
    ram[index] = value % 10;
}

void Chip8::opFx55() {
    for (int i = 0; i <= getVx(); i++)
        ram[index + i] = registers[i];
}

void Chip8::opFx65() {
    for (int i = 0; i <= getVx(); i++)
        registers[i] = ram[index + i];
}

void Chip8::opNOP() {
}

uint8_t Chip8::getVx() const {
    return (opcode & 0x0F00) >> 8;
}

uint8_t Chip8::getVy() const {
    return (opcode & 0x00F0) >> 4;
}
