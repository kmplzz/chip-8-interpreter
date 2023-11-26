#include <chrono>

#include "src/Chip8.h"
#include "src/Renderer.h"

constexpr int SCREEN_SCALE = 20;
constexpr int DELAY = 5;

int main() {
    Chip8 chip8;
    chip8.loadRom("/home/kmplzz/space_Invaders.ch8");
    Renderer renderer(chip8.framebuffer, chip8.keyboard, SCREEN_SCALE);
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    while (renderer.processInput()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > DELAY) {
            lastCycleTime = currentTime;
            chip8.cycle();
            renderer.update();
        }
    }

}
