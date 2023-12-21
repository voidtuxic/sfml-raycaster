//
// Created by tuxic on 21/12/2023.
//

#ifndef WALL_H
#define WALL_H
#include <SFML/Graphics.hpp>
#include "Config.h"

inline void calculateStep(const sf::Vector2<double> &position, const sf::Vector2<double> &rayDir,
                          const sf::Vector2<double> &deltaDist, const sf::Vector2i &mapPosition, sf::Vector2<double> &sideDist,
                          sf::Vector2i &step) {
    //what direction to step in x or y-direction (either +1 or -1)
    if (rayDir.x < 0) {
        step.x = -1;
        sideDist.x = (position.x - mapPosition.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (mapPosition.x + 1.0 - position.x) * deltaDist.x;
    }
    if (rayDir.y < 0) {
        step.y = -1;
        sideDist.y = (position.y - mapPosition.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (mapPosition.y + 1.0 - position.y) * deltaDist.y;
    }
}

inline void performDDA(const int map[MAP_WIDTH][MAP_HEIGHT], int &side, const sf::Vector2<double> &deltaDist,
                       const sf::Vector2i &step, sf::Vector2i &mapPosition, sf::Vector2<double> &sideDist) {
    int hit = 0; //was there a wall hit?
    while (hit == 0) {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDist.x < sideDist.y) {
            sideDist.x += deltaDist.x;
            mapPosition.x += step.x;
            side = step.x > 0 ? 0 : 2;
        } else {
            sideDist.y += deltaDist.y;
            mapPosition.y += step.y;
            side = step.y > 0 ? 1 : 3;
        }
        //Check if ray has hit a wall
        if (map[mapPosition.x][mapPosition.y] > 0) hit = 1;
    }
}

inline void calculateWall(const int map[MAP_WIDTH][MAP_HEIGHT], const sf::Vector2<double> &position,
                          const sf::Vector2<double> &direction, const sf::Vector2<double> plane, const int x,
                          sf::Vector2<double> &rayDir, sf::Vector2i &mapPosition, double &perpWallDist, int &side,
                          int &lineHeight, int &drawStart, int &drawEnd) {
    //calculate ray position and direction
    const double cameraX = 2 * x / static_cast<double>(RENDER_WIDTH) - 1; //x-coordinate in camera space
    rayDir = sf::Vector2(direction.x + plane.x * cameraX, direction.y + plane.y * cameraX);
    mapPosition = sf::Vector2i(static_cast<int>(position.x), static_cast<int>(position.y));

    //length of ray from current position to next x or y-side
    sf::Vector2<double> sideDist;
    sf::Vector2i step;

    //length of ray from one x or y-side to next x or y-side
    const sf::Vector2<double> deltaDist = sf::Vector2<double>(
        (rayDir.x == 0) ? 1e30 : std::abs(1 / rayDir.x),
        (rayDir.y == 0) ? 1e30 : std::abs(1 / rayDir.y));

    //calculate step and initial sideDist
    calculateStep(position, rayDir, deltaDist, mapPosition, sideDist, step);

    //perform DDA
    performDDA(map, side, deltaDist, step, mapPosition, sideDist);

    //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    if (side % 2 == 0) perpWallDist = (sideDist.x - deltaDist.x);
    else perpWallDist = (sideDist.y - deltaDist.y);
    lineHeight = static_cast<int>(RENDER_HEIGHT / perpWallDist);

    drawStart = -lineHeight / 2 + RENDER_HEIGHT / 2;
    if (drawStart < 0) drawStart = 0;
    drawEnd = lineHeight / 2 + RENDER_HEIGHT / 2;
    if (drawEnd >= RENDER_HEIGHT) drawEnd = RENDER_HEIGHT - 1;
}

#endif //WALL_H
