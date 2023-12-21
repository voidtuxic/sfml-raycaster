//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>

class App {
public:
    App();
    ~App();
    int run();

private:
    sf::RenderWindow *window;
    sf::Clock deltaClock;
    sf::Uint8 *buffer;
    sf::Texture *texture;
    sf::RectangleShape *bufferRect;

    std::vector<sf::Image *> textures;

    sf::Vector2<double> position = sf::Vector2<double>(22, 11.5);
    sf::Vector2<double> direction = sf::Vector2<double>(-1, 0);
    sf::Vector2<double> plane = sf::Vector2<double>(0, 0.9);

    sf::Vector2i previousMousePosition;

private:
    void handleInput();
    void render() const;
};


#endif //APP_H
