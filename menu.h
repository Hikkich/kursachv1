using namespace sf;
#include <Windows.h>
#include "classes.h"



//ф-ция для настройки текста
void InitText(Text& mtext, float xpos, float ypos, String str, int size_font = 60,
    Color menu_text_color = Color::White, int bord = 0, Color border_color = Color::Black)
{
    mtext.setCharacterSize(size_font);
    mtext.setPosition(Vector2f(xpos, ypos));
    mtext.setString(str);
    mtext.setFillColor(menu_text_color);
    mtext.setOutlineThickness(bord);
    mtext.setOutlineColor(border_color);
}

RoundedRectangleShape createRoundedRect(float width, float height, float radius, const sf::Color& color) {
    RoundedRectangleShape shape({ width, height }, radius, 30);
    shape.setFillColor(color);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(150, 150, 150));
    return shape;
}


int gamestart()
{
    //Окно windows
    RenderWindow window;
    window.create(VideoMode::getDesktopMode(), L"Shinzou Rizumu", State::Windowed);

    //фон
    RectangleShape background(Vector2f(1920, 1080));
    Texture texture_window;
    if (!texture_window.loadFromFile("images/background.jpg")) return 4;
    background.setTexture(&texture_window);

    RectangleShape dashka(Vector2f(1920, 1080));
    Texture dashkatexture;
    if (!dashkatexture.loadFromFile("images/daska2.jpg")) return 4;
    dashka.setTexture(&dashkatexture);

    RectangleShape scene1(Vector2f(1920, 1080));
    Texture tscene1;
    if (!tscene1.loadFromFile("images/daska2.jpg")) return 4; // Измените на ваше изображение сцены
    scene1.setTexture(&tscene1);

    //music
    sf::Music music;
    if (!music.openFromFile("by.wav"))
        return -1; // error 
    sf::Music music1;
    if (!music1.openFromFile("pony.wav"))
        return -1; // error

    //name
    Font font;
    Font font1;
    if (!font.openFromFile("Hot_20Mustard_20BTN.ttf"))
        Text Titul(font);//если 1 то не работает(
    Text Titul(font);
    Text Titul1(font);
    Text Titul2(font);
    //game name
    InitText(Titul, 210, 90, L"Shinzou", 70, Color(255, 182, 193), 3);
    Titul.setRotation(degrees(-5));
    InitText(Titul1, 240, 170, L"Rizumu", 60, Color(255, 182, 193), 3);
    Titul1.setRotation(degrees(-5));
    if (!font1.openFromFile("Dj5ctrial.ttf"))
        Text Titullit(font1);//если 1 то не работает(
    Text Titullit(font1);
    InitText(Titullit, 250, 245, L"Literature Club", 40, Color(255, 255, 255), 3, Color(0, 0, 0));
    Titullit.setRotation(degrees(-5));
    //controls
    Text newgame(font);
    InitText(newgame, 150, 545, L"New Game", 40, Color(255, 255, 255), 6, Color(255, 182, 193));
    Text loadgame(font);
    InitText(loadgame, 150, 600, L"Load Game", 40, Color(255, 255, 255), 6, Color(255, 182, 193));
    Text settings(font);
    InitText(settings, 150, 655, L"Settings", 40, Color(255, 255, 255), 6, Color(255, 182, 193));
    Text help(font);
    InitText(help, 150, 710, L"Help", 40, Color(255, 255, 255), 6, Color(255, 182, 193));
    Text quit(font);
    InitText(quit, 150, 765, L"Quit", 40, Color(255, 255, 255), 6, Color(255, 182, 193));

    //effects
    FadeEffect fade;
    fade.fadeShape.setSize(sf::Vector2f(window.getSize()));
    fade.fadeShape.setFillColor(sf::Color::Transparent);

    bool isMenuActive = true;
    bool isFading = false;

    sf::SoundBuffer kissBuffer;
    if (!kissBuffer.loadFromFile("kiss.mp3")) {
        MessageBoxA(nullptr, "Не удалось загрузить kiss.mp3", "Ошибка", MB_OK | MB_ICONERROR);
        return -1;
    }
    sf::Sound kissSound(kissBuffer);
    kissSound.setVolume(10.f);
    std::vector<Texture> introFrames;
    std::vector<Sprite> introSprites;

    for (int i = 0; i <= 42; ++i) // от 0 до 42
    {
        Texture texture;
        std::ostringstream filename;
        filename << "intro/frame_"
            << std::setfill('0') << std::setw(2) << i
            << "_delay-0.07s.png";

        if (!texture.loadFromFile(filename.str()))
        {
            std::cerr << "Не удалось загрузить кадр: " << filename.str() << std::endl;
            continue;
        }

        introFrames.push_back(texture);
    }

    for (const auto& tex : introFrames)
    {
        Sprite sprite(tex);

        sf::Vector2u texSize = tex.getSize();
        sf::Vector2u winSize = window.getSize();

        float scaleX = static_cast<float>(winSize.x) / texSize.x;
        float scaleY = static_cast<float>(winSize.y) / texSize.y;
        sprite.setScale(Vector2f(scaleX, scaleY));

        introSprites.push_back(sprite);
    }
    kissSound.play();

    sf::Text loadingText(font);
    InitText(loadingText, window.getSize().x / 2.f, window.getSize().y - 300.f, L"Loading...", 60, Color::White, 3, Color::Black);
    sf::FloatRect bounds = loadingText.getLocalBounds();
    loadingText.setOrigin(Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));

    float loadingAlpha = 255.f;  // Начальная прозрачность
    float loadingAlphaSpeed = -200.f; // скорость изменения прозрачности (пикс/сек)
    sf::Clock loadingClock; // таймер для обновления альфы

    DialogueBox dialogBox({ 100.f, 800.f }, { 1720.f, 200.f });

    for (const auto& sprite : introSprites)
    {

        float dt = loadingClock.restart().asSeconds();
        loadingAlpha += loadingAlphaSpeed * dt;

        if (loadingAlpha < 50.f)  // минимальная прозрачность
        {
            loadingAlpha = 50.f;
            loadingAlphaSpeed = -loadingAlphaSpeed; // инвертируем направление
        }
        else if (loadingAlpha > 255.f)  // максимальная прозрачность
        {
            loadingAlpha = 255.f;
            loadingAlphaSpeed = -loadingAlphaSpeed; // инвертируем направление
        }

        loadingText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(loadingAlpha)));
        loadingText.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(loadingAlpha)));

        window.clear();
        window.draw(sprite);
        window.draw(loadingText);
        window.display();
        sf::sleep(sf::milliseconds(70)); // 70 мс задержка
    }

    kissSound.stop();

    enum class GameState
    {
        Menu,
        Scene1,
        Scene2,
        Scene3,
    };
    GameState gameState = GameState::Menu;
    GameState nextGameState = gameState;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                if (gameState != GameState::Menu) {
                    gameState = GameState::Menu;
                }
            }


            if (gameState == GameState::Menu && !isFading)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                //newgame
                sf::FloatRect buttonnewgame = newgame.getGlobalBounds();
                if (buttonnewgame.contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    newgame.setScale(Vector2f(1.2, 1.2));
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                        {
                            isFading = true;
                            fade.start();
                            nextGameState = GameState::Scene1;
                        }
                    }
                }
                else
                {
                    newgame.setScale(Vector2f(1, 1));
                }

                //load game
                sf::FloatRect buttonloadgame = loadgame.getGlobalBounds();
                if (buttonloadgame.contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    loadgame.setScale(Vector2f(1.2, 1.2));
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                            /*menu = false;*/
                        }
                    }
                }
                else
                {

                    loadgame.setScale(Vector2f(1, 1));
                }

                //settings
                sf::FloatRect buttonsettings = settings.getGlobalBounds();
                if (buttonsettings.contains(static_cast<sf::Vector2f>(mousePos)))
                {

                    settings.setScale(Vector2f(1.2, 1.2));
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                        }
                    }
                }
                else
                {

                    settings.setScale(Vector2f(1, 1));
                }

                //help
                sf::FloatRect buttonhelp = help.getGlobalBounds();
                if (buttonhelp.contains(static_cast<sf::Vector2f>(mousePos)))
                {

                    help.setScale(Vector2f(1.2, 1.2));
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                        }
                    }
                }
                else
                {

                    help.setScale(Vector2f(1, 1));
                }

                //quit
                sf::FloatRect buttonquit = quit.getGlobalBounds();
                if (buttonquit.contains(static_cast<sf::Vector2f>(mousePos)))
                {
                    quit.setFillColor(sf::Color::Red);
                    quit.setScale(Vector2f(1.2, 1.2));
                    music1.play();
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                            music1.stop();
                        music.setVolume(1000.f);
                        music.play();
                        exit(0);
                    }
                }
                else
                {
                    quit.setFillColor(Color(255, 255, 255));
                    quit.setScale(Vector2f(1, 1));
                    music1.stop();
                }
            }
        }

        if (gameState == GameState::Scene1) {
            dialogBox.update();
        }


        window.clear();


        switch (gameState)
        {
        case GameState::Menu:
            window.draw(background);
            window.draw(Titul);
            window.draw(Titul1);
            window.draw(Titul2);
            window.draw(Titullit);
            window.draw(newgame);
            window.draw(loadgame);
            window.draw(settings);
            window.draw(help);
            window.draw(quit);
            break;

        case GameState::Scene1:
            window.draw(scene1);

            dialogBox.draw(window);
            break;

            //case GameState::Scene2:
            //    window.draw(scene2);
            //    // и другой диалог
            //    break;

            //case GameState::Scene3:
            //    window.draw(scene3);
            //    break;
        }

        //if (isMenuActive)
        //{
        //    // Рисуем меню
        //    window.draw(background);
        //    window.draw(Titul);
        //    window.draw(Titul1);
        //    window.draw(Titul2);
        //    window.draw(Titullit);
        //    window.draw(newgame);
        //    window.draw(loadgame);
        //    window.draw(settings);
        //    window.draw(help);
        //    window.draw(quit);
        //}
        //else
        //{
        //    // Рисуем игровую сцену
        //    window.draw(scene1);
        //}


        // Обработка эффекта затухания
        //if (isFading)
        //{
        //    fade.update();
        //    fade.draw(window);

        //    if (fade.isComplete())
        //    {
        //        isFading = false;
        //        isMenuActive = false; // Переключаемся на игровую сцену
        //    }
        //}

        if (isFading)
        {
            fade.update();
            fade.draw(window);

            if (fade.isComplete())
            {
                isFading = false;
                gameState = nextGameState;
                if (gameState == GameState::Scene1)
                {
                    dialogBox.setText(L"Привет! Добро пожаловать в нашу игру! Пожалуйста, наслаждайся сюжетом...");
                }
            }
        }


        window.display();
    }
}