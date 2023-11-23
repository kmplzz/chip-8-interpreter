#include "src/Chip8.h"
#include "src/Renderer.h"

constexpr int SCREEN_SCALE = 10;

int main() {
    Chip8 chip8;
    Renderer renderer(chip8.video, chip8.keyboard, SCREEN_SCALE);
    while (renderer.processInput()) {
        chip8.cycle();
        renderer.update();
    }
}
