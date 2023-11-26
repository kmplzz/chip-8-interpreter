#include <chrono>

#include "src/Chip8.h"
#include "src/Renderer.h"

using namespace std;

constexpr int SCREEN_SCALE = 20;
constexpr int DELAY = 1;

int main() {
    Chip8 chip8;
    chip8.load("/home/kmplzz/pong.ch8");

    Renderer renderer(Chip8::SCREEN_WIDTH, Chip8::SCREEN_HEIGHT, SCREEN_SCALE, Color::Black, Color::Magenta);

    auto lastCycleTime = chrono::high_resolution_clock::now();

    while (renderer.input(chip8.keyboard)) {
        auto now = chrono::high_resolution_clock::now();
        auto millisHavePassed = chrono::duration<float, chrono::milliseconds::period>(now - lastCycleTime).count();

        if (millisHavePassed < DELAY)
            continue;

        lastCycleTime = now;
        chip8.cycle();
        renderer.update(chip8.framebuffer);
    }
}
