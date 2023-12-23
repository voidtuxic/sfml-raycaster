//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>

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

    sf::Texture *texture;
    sf::RectangleShape *bufferRect;
    wolf::RenderData *renderData;

    wolf::CameraData* camera;

    sf::Vector2i previousMousePosition;

private:
    void loadTexture(const std::string & filename) const;

    void handleKeyboard(double frameTime, double totalTime) const;

    void handleMouse(double frameTime);

    void handleInput();

    void render() const;
};


#endif //APP_H
