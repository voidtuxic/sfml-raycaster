//
// Created by tuxic on 21/12/2023.
//

#ifndef FLOOR_H
#define FLOOR_H
#include <SFML/Graphics.hpp>
#include "Config.h"

inline void calculateFloor(const sf::Vector2<double> &position, const sf::Vector2<double> &direction,
                           const sf::Vector2<double> plane, const int y, sf::Vector2<double> &floorStep,
                           sf::Vector2<double> &floor, double &rowDistance) {
    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    const auto rayDir0 = sf::Vector2(direction.x - plane.x, direction.y - plane.y);
    const auto rayDir1 = sf::Vector2(direction.x + plane.x, direction.y + plane.y);

    // Current y position compared to the center of the screen (the horizon)
    const int p = y - RENDER_HEIGHT / 2;

    // Vertical position of the camera.
    constexpr double posZ = 0.5 * RENDER_HEIGHT;

    // Horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    rowDistance = posZ / p;

    floorStep = sf::Vector2(rowDistance * (rayDir1.x - rayDir0.x) / RENDER_WIDTH,
                            rowDistance * (rayDir1.y - rayDir0.y) / RENDER_WIDTH);

    floor = sf::Vector2(position.x + rowDistance * rayDir0.x, position.y + rowDistance * rayDir0.y);
}
#endif //FLOOR_H
