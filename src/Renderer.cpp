#include "Renderer.h"

const std::string WINDOW_TITLE = "CHIP-8 interpreter";

Renderer::Renderer(const int width, const int height, const int scale, const Color bgColor, const Color fgColor) {
    this->bgColor = bgColor;
    this->fgColor = fgColor;

    const VideoMode videoMode(width * scale, height * scale);
    window = new RenderWindow(videoMode, WINDOW_TITLE);

    texture = new RenderTexture();
    texture->create(Chip8::SCREEN_WIDTH, Chip8::SCREEN_HEIGHT);

    sprite = new Sprite(texture->getTexture());
    const auto scaleF = static_cast<float>(scale);
    sprite->setScale(Vector2f(scaleF, scaleF));
}

Renderer::~Renderer() {
    delete sprite;
    delete texture;
    delete window;
};

bool Renderer::input(std::array<uint8_t, Chip8::KEYS_COUNT> &keyboard) {
    if (!window->isOpen()) return false;

    while (window->pollEvent(event)) {
        switch (event.type) {
            case Event::Closed:
                window->close();
                break;
            case Event::KeyPressed:
            case Event::KeyReleased:
                try {
                    const uint8_t keyIdx = keyCodes.at(event.key.code);
                    keyboard[keyIdx] = (event.type == Event::KeyPressed ? 1 : 0);
                }
                catch (std::out_of_range&) {}
                break;
            default:
                break;
        }
    }

    return true;
}

void Renderer::update(const std::array<std::array<uint8_t, Chip8::SCREEN_HEIGHT>, Chip8::SCREEN_WIDTH> &framebuffer) const {
    window->clear();

    std::vector<Vertex> pixels;
    for (int i = 0; i < framebuffer.size(); i++) {
        for (int j = 0; j < framebuffer[i].size(); j++) {
            const int pixel = framebuffer[i][j];

            const auto x = static_cast<float>(i);
            const auto y = Chip8::SCREEN_HEIGHT - static_cast<float>(j);
            Vector2f coords(x, y);

            Color color = pixel == 0 ? bgColor : fgColor;

            pixels.emplace_back(coords, color);
        }
    }

    texture->draw(pixels.data(), pixels.size(), Points);
    window->draw(*sprite);
    window->display();
}
