#include "Renderer.h"

const std::string WINDOW_TITLE = "CHIP-8 interpreter";

Renderer::Renderer(uint8_t (&framebuffer)[Chip8::SCREEN_WIDTH][Chip8::SCREEN_HEIGHT],
                   uint8_t (&keyboard)[Chip8::KEYS_COUNT],
                   const int scale) : m_framebuffer(framebuffer), m_keyboard(keyboard) {

        const sf::VideoMode videoMode(Chip8::SCREEN_WIDTH * scale, Chip8::SCREEN_HEIGHT * scale);
        m_window = new sf::RenderWindow(videoMode, WINDOW_TITLE);

        m_texture = new sf::RenderTexture();
        m_texture->create(Chip8::SCREEN_WIDTH, Chip8::SCREEN_HEIGHT);

        m_sprite = new sf::Sprite(m_texture->getTexture());
        const auto scaleF = static_cast<float>(scale);
        m_sprite->setScale(sf::Vector2f(scaleF, scaleF));
}

Renderer::~Renderer() {
    delete m_sprite;
    delete m_texture;
    delete m_window;
};

bool Renderer::processInput() {
    if (!m_window->isOpen()) return false;
    while (m_window->pollEvent(m_event)) {
        switch (m_event.type) {
            case sf::Event::Closed:
                m_window->close();
                break;
            case sf::Event::KeyPressed:
            case sf::Event::KeyReleased:
            try {
                const uint8_t keyIdx = keyCodes.at(m_event.key.code);
                m_keyboard[keyIdx] = (m_event.type == sf::Event::KeyPressed ? 1 : 0);
            } catch (std::out_of_range& ex) {
                // ignore
            }
                break;
            default:
                break;
        }
    }
    return true;
}

void Renderer::update() const {
    m_window->clear();

    std::vector<sf::Vertex> pixels;
    for (int i = 0; i < Chip8::SCREEN_WIDTH; i++) {
        for (int j = 0; j < Chip8::SCREEN_HEIGHT; j++) {
            const int pixel = m_framebuffer[i][j];

            const auto x = static_cast<float>(i);
            const auto y = Chip8::SCREEN_HEIGHT - static_cast<float>(j);
            sf::Vector2f coords(x, y);

            sf::Color color = pixel == 0 ? sf::Color::Black : sf::Color::Green;

            pixels.emplace_back(coords, color);
        }
    }

    m_texture->draw(pixels.data(), Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT, sf::Points);
    m_window->draw(*m_sprite);
    m_window->display();
}
