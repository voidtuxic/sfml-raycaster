//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TEX_WIDTH 128
#define TEX_HEIGHT 128
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define RENDER_WIDTH 640
#define RENDER_HEIGHT 360
#define RENDER_COMPONENTS 4
#define RENDER_STRIDE RENDER_WIDTH * RENDER_COMPONENTS
#define WALL_DISTANCE 8
#define FLOOR_TEXTURE 8
#define CEILING_TEXTURE 9
#define FOG_DISTANCE 10

inline int worldMap[MAP_WIDTH][MAP_HEIGHT] =
{
    {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
    {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
    {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
    {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
    {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}
};

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

    void setColor(int x, int y, sf::Color color) const;

    void clearBuffer() const;

    auto calculateStep(const sf::Vector2<double> &rayDir, const sf::Vector2<double> &deltaDist,
                       const sf::Vector2i &map, sf::Vector2<double> &sideDist,
                       sf::Vector2i &step) const -> void;

    static auto performDDA(int &side, const sf::Vector2<double> &deltaDist, const sf::Vector2i &step, sf::Vector2i &map,
                           sf::Vector2<double> &sideDist) -> void;

    auto calculate(int x, sf::Vector2<double> &rayDir, sf::Vector2i &map, double &perpWallDist,
                   int &side, int &lineHeight, int &drawStart, int &drawEnd) const -> void;

    void calculateFloor(int y, sf::Vector2<double> &floorStep, sf::Vector2<double> &floor, double &rowDistance) const;

    void drawColumn(int x, int side, int drawStart, int drawEnd, int texNum, int texX, double step,
                    double texPos, double perpWallDist) const;

    void drawFloorAndCeiling(int y, sf::Vector2<double> floorStep, sf::Vector2<double> floor, double &rowDistance) const;

    auto getTextureParameters(const sf::Vector2<double> &rayDir, const sf::Vector2i &map, double perpWallDist, int side,
                              int lineHeight, int drawStart, int &texNum, int &texX, double &step,
                              double &texPos) const -> void;

    static void applyFog(double distance, sf::Color &color);

    void render() const;
};


#endif //APP_H
