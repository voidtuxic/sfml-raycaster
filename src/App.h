//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>
#include <glm/vec2.hpp>

#include "Render.h"

class App {
public:
    App();
    ~App();
    int run();

private:
    sf::RenderWindow *window;
    sf::Clock deltaClock;
    sf::Clock clock;
    sf::Clock renderDeltaClock;

    sf::Texture *texture;
    sf::RectangleShape *bufferRect;
    wolf::RenderData *renderData;

    wolf::CameraData* camera;

    glm::ivec2 previousMousePosition{};

    sf::Font debugFont;
    sf::Text *debugText;
    int renderTimeMS = 0;
private:
    void loadTexture(const std::string & filename) const;

    void handleKeyboard(float frameTime, float totalTime) const;

    void handleMouse(float frameTime);

    void handleInput();

    void render();
};


#endif //APP_H
