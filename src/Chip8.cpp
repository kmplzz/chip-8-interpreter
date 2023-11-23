#include "Chip8.h"

Chip8::Chip8() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video[i] = i % 2;
    }
    video[0] = 2;
    video[5] = 2;
}

void Chip8::cycle() {
}
