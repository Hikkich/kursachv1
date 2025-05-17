#pragma once
using namespace sf;
#include <Windows.h>
#include"effects.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>



struct FadeEffect {
    RectangleShape fadeShape;
    float fadeAlpha = 0.f;
    bool isActive = false;
    float duration = 2.f;
    Clock clock;

    void start(float fadeDuration = 2.f) {
        duration = fadeDuration;
        fadeAlpha = 0.f;
        isActive = true;
        clock.restart();
    }

    void update() {
        if (!isActive) return;

        float progress = clock.getElapsedTime().asSeconds() / duration;
        fadeAlpha = progress * 255.f;

        if (progress >= 1.f) {
            fadeAlpha = 255.f;
            isActive = false;
        }

        fadeShape.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(fadeAlpha)));
    }

    void draw(sf::RenderWindow& window) {
        if (!isActive) return;
        window.draw(fadeShape);
    }

    bool isComplete() const { return !isActive; }
};

struct TextAnimation {
    Text text;
    float duration;
    float progress = 0.f;
    Clock clock;
    bool isActive = false;

    void start(const String& str, float animDuration, const Font& font, unsigned int size) {
        text.setString(str);
        text.setFont(font);
        text.setCharacterSize(size);
        duration = animDuration;
        progress = 0.f;
        isActive = true;
        clock.restart();
    }

    void update() {
        if (!isActive) return;

        progress = std::min(clock.getElapsedTime().asSeconds() / duration, 1.0f);

        // Анимация появления (можно изменить на другой эффект)
        text.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(progress * 255)));

        if (progress >= 1.f) {
            isActive = false;
        }
    }

    void draw(RenderWindow& window) const {
        if (progress > 0.f) {
            window.draw(text);
        }
    }
};

struct FrameAnimation {
    std::vector<Texture> frames;
    std::optional<Sprite> sprite;
    std::optional<Sound> sound;
    SoundBuffer soundBuffer;
    float frameDuration = 0.07f; // секунды
    std::size_t currentFrame = 0;
    Clock frameClock;

    bool isPlaying = false;

    void loadFrames(const std::string& folderPath, int frameCount, RenderWindow& window) {
        frames.clear();
        for (int i = 0; i < frameCount; ++i) {
            char filename[256];
            sprintf_s(filename, "frame_%02d_delay-0.07s.png", i);

            Texture texture;
            if (texture.loadFromFile(folderPath + "/" + filename)) {
                frames.push_back(std::move(texture));
            }
            else {
                MessageBoxA(nullptr, filename, "Ошибка загрузки кадра", MB_OK | MB_ICONERROR);
            }
        }

        if (!frames.empty()) {
            sprite.emplace(frames[0]);
            resizeSprite(window);
        }
    }

    void resizeSprite(RenderWindow& window) {
        if (!sprite || frames.empty()) return;

        float scaleX = static_cast<float>(window.getSize().x) / frames[0].getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / frames[0].getSize().y;
        sprite->setScale(sf::Vector2f(scaleX, scaleY));
    }

    void loadSound(const std::string& soundPath) {
        if (soundBuffer.loadFromFile(soundPath)) {
            sound.emplace(soundBuffer);
        }
    }

    void playSound() {
        if (sound) {
            sound->play();
        }
    }

    void start() {
        isPlaying = true;
        currentFrame = 0;
        frameClock.restart();
    }

    void update(RenderWindow& window) {
        if (!isPlaying || frames.empty()) return;

        if (frameClock.getElapsedTime().asSeconds() >= frameDuration) {
            frameClock.restart();
            ++currentFrame;
            if (currentFrame >= frames.size()) {
                isPlaying = false; // Анимация закончилась
                currentFrame = frames.size() - 1; // Остановиться на последнем кадре
            }
            if (sprite) {
                sprite->setTexture(frames[currentFrame]);
                resizeSprite(window);
            }
        }
    }

    void draw(RenderWindow& window) {
        if (sprite && isPlaying) {
            window.draw(*sprite);
        }
    }
};

struct Rect
{
    Vector2f position; // Позиция (x, y)
    Vector2f size;     // Размер (width, height)
};

class RoundedRectangleShape : public sf::Shape {
public:
    explicit RoundedRectangleShape(const sf::Vector2f& size = { 0.f, 0.f },
        float radius = 0.f,
        unsigned int cornerPointCount = 10)
        : m_size(size), m_radius(radius), m_cornerPointCount(cornerPointCount) {
        update();
    }

    void setSize(const sf::Vector2f& size) {
        m_size = size;
        update();
    }

    const sf::Vector2f& getSize() const {
        return m_size;
    }

    void setCornersRadius(float radius) {
        m_radius = radius;
        update();
    }

    float getCornersRadius() const {
        return m_radius;
    }

    void setCornerPointCount(unsigned int count) {
        m_cornerPointCount = count;
        update();
    }

    std::size_t getPointCount() const override {
        return m_cornerPointCount * 4;
    }

    sf::Vector2f getPoint(std::size_t index) const override {
        if (index >= m_cornerPointCount * 4)
            return { 0.f, 0.f };

        const float deltaAngle = 90.f / (m_cornerPointCount - 1);
        sf::Vector2f center;
        const unsigned int centerIndex = index / m_cornerPointCount;
        constexpr static float pi = 3.14159265358979f;

        switch (centerIndex) {
        case 0: center = { m_size.x - m_radius, m_radius }; break;
        case 1: center = { m_radius, m_radius }; break;
        case 2: center = { m_radius, m_size.y - m_radius }; break;
        case 3: center = { m_size.x - m_radius, m_size.y - m_radius }; break;
        }

        const float angle = deltaAngle * (index - centerIndex * m_cornerPointCount) * pi / 180.f;
        return { m_radius * std::cos(angle) + center.x,
                -m_radius * std::sin(angle) + center.y };
    }

private:
    sf::Vector2f m_size;
    float m_radius;
    unsigned int m_cornerPointCount;
};

class DialogueBox {
private:
    RoundedRectangleShape background;
    sf::Font font;
    sf::Text text; // инициализация через конструктор ниже
    std::wstring fullText;
    std::wstring currentText;
    sf::Clock typingClock;
    float typingSpeed;
    std::size_t currentIndex = 0;
    bool isFinished = false;


public:
    DialogueBox(const sf::Vector2f& position, const sf::Vector2f& size, float borderRadius = 30.f)
        : font(), text(font), typingSpeed(30.f)
    {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            throw std::runtime_error("Не удалось загрузить шрифт!");
        }

        text.setCharacterSize(28);
        text.setFillColor(sf::Color::White);
        text.setPosition(position + sf::Vector2f(20.f, 20.f));

        background = RoundedRectangleShape(size, borderRadius, 30);
        background.setFillColor(sf::Color(0, 0, 0, 180));
        background.setOutlineThickness(2.f);
        background.setOutlineColor(sf::Color::White);
        background.setPosition(position);
    }
    void setText(const std::wstring& str) {
        fullText = str;
        currentText.clear();
        currentIndex = 0;
        isFinished = false;
        typingClock.restart();
    }

    bool isFullyDisplayed() const;
    void finishText();

    void update() {
        if (isFinished) return;

        float elapsed = typingClock.getElapsedTime().asSeconds();
        std::size_t expectedIndex = static_cast<std::size_t>(elapsed * typingSpeed);

        if (expectedIndex > currentIndex && currentIndex < fullText.size()) {
            currentIndex++;
            currentText = fullText.substr(0, currentIndex);
            text.setString(currentText);
        }

        if (currentIndex >= fullText.size()) {
            isFinished = true;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(text);
    }

    bool finished() const {
        return isFinished;
    }

};