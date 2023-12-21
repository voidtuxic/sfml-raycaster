//
// Created by tuxic on 21/12/2023.
//

#ifndef FLOOR_H
#define FLOOR_H
#include <SFML/Graphics.hpp>

inline void calculateFloorWall(const sf::Vector2<double> rayDir, const sf::Vector2i mapPosition, const double wallX,
                               const int side, sf::Vector2<double> &floorWall) {
    //4 different wall directions possible
    if(side % 2 == 0 && rayDir.x > 0)
    {
        floorWall.x = mapPosition.x;
        floorWall.y = mapPosition.y + wallX;
    }
    else if(side % 2 == 0 && rayDir.x < 0)
    {
        floorWall.x = mapPosition.x + 1.0;
        floorWall.y = mapPosition.y + wallX;
    }
    else if(side % 2 == 1 && rayDir.y > 0)
    {
        floorWall.x = mapPosition.x + wallX;
        floorWall.y = mapPosition.y;
    }
    else
    {
        floorWall.x = mapPosition.x + wallX;
        floorWall.y = mapPosition.y + 1.0;
    }
}
#endif //FLOOR_H
