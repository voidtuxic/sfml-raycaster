//
// Created by tuxic on 21/12/2023.
//

#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>

#define mapWidth 24
#define mapHeight 24
#define texWidth 64
#define texHeight 64
#define screenWidth 640
#define screenHeight 480
#define wallDistance 8

inline int worldMap[mapWidth][mapHeight]=
{
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
    {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
    {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
    {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
    {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
    {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
    {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
    {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
    {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
    {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

class App {
public:
    int run();
    App();
    ~App();

private:
    sf::RenderWindow* window;
    sf::Clock deltaClock;
    sf::Image* buffer;
    sf::Texture* texture;
    sf::Sprite* bufferSprite;

    std::vector<sf::Image*> textures;

    double posX = 22;
    double posY = 11.5;
    double dirX = -1;
    double dirY = 0;
    double planeX = 0;
    double planeY = 0.66;

private:
    void handleInput();

    void clearBuffer(int x) const;

    void calculateStep(const double &rayDirX, const double &rayDirY, const int &mapX, const int &mapY,
                       double &sideDistX, double &sideDistY, double deltaDistX, double deltaDistY,
                       int &stepX, int &stepY) const;

    static void performDDA(int &mapX, int &mapY, int &side, double &sideDistX, double &sideDistY, double deltaDistX,
                           double deltaDistY, int stepX, int stepY);

    void calculate(int x, double &rayDirX, double &rayDirY, int &mapX, int &mapY, double &perpWallDist,
                   int &side, int &lineHeight, int &drawStart, int &drawEnd) const;

    void drawColumn(int x, int side, int drawStart, int drawEnd, int texNum, int texX, double step,
                    double texPos) const;

    void getTextureParameters(double rayDirX, double rayDirY, int mapX, int mapY, double perpWallDist, int side,
                              int lineHeight, int drawStart, int &texNum, int &texX, double &step,
                              double &texPos) const;

    void render() const;
};



#endif //APP_H
