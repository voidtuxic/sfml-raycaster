//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>

class App {
public:
    int run();

    App();

    ~App();

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

private:
    void handleInput();

    void drawColumn(int x, int side, int drawStart, int drawEnd, int texNum, int texX, double step,
                    double texPos, double perpWallDist) const;

    void drawFloorAndCeiling(int y, sf::Vector2<double> floorStep, sf::Vector2<double> floor, const double &rowDistance) const;

    auto getTextureParameters(const sf::Vector2<double> &rayDir, const sf::Vector2i &map, double perpWallDist, int side,
                              int lineHeight, int drawStart, int &texNum, int &texX, double &step,
                              double &texPos) const -> void;

    void render() const;
};


#endif //APP_H
