#include "Renderer.h"

const std::string WINDOW_TITLE = "CHIP-8 interpreter";

Renderer::Renderer(uint8_t (&video)[Chip8::SCREEN_WIDTH][Chip8::SCREEN_HEIGHT],
                   uint8_t (&keyboard)[Chip8::KEYS_COUNT],
                   const int scale) : m_video(video), m_keyboard(keyboard) {

        const sf::VideoMode videoMode(Chip8::SCREEN_WIDTH * scale, Chip8::SCREEN_HEIGHT * scale);
        m_window = new sf::RenderWindow(videoMode, WINDOW_TITLE);
        m_window->setFramerateLimit(60);

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
        if (m_event.type == sf::Event::Closed) {
            m_window->close();
        }
    }
    return true;
}

void Renderer::update() const {
    m_window->clear();

    std::vector<sf::Vertex> pixels;
    for (int i = 0; i < Chip8::SCREEN_WIDTH; i++) {
        for (int j = 0; j < Chip8::SCREEN_HEIGHT; j++) {
            const int pixel = m_video[i][j];

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
