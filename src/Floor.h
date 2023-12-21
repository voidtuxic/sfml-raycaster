//
// Created by tuxic on 21/12/2023.
//

#ifndef FLOOR_H
#define FLOOR_H
#include <SFML/Graphics.hpp>

inline void calculateFloorWall(RaycastData &raycast) {
    //4 different wall directions possible
    if(raycast.side % 2 == 0 && raycast.rayDirection.x > 0)
    {
        raycast.floorWall.x = raycast.mapPosition.x;
        raycast.floorWall.y = raycast.mapPosition.y + raycast.wallX;
    }
    else if(raycast.side % 2 == 0 && raycast.rayDirection.x < 0)
    {
        raycast.floorWall.x = raycast.mapPosition.x + 1.0;
        raycast.floorWall.y = raycast.mapPosition.y + raycast.wallX;
    }
    else if(raycast.side % 2 == 1 && raycast.rayDirection.y > 0)
    {
        raycast.floorWall.x = raycast.mapPosition.x + raycast.wallX;
        raycast.floorWall.y = raycast.mapPosition.y;
    }
    else
    {
        raycast.floorWall.x = raycast.mapPosition.x + raycast.wallX;
        raycast.floorWall.y = raycast.mapPosition.y + 1.0;
    }
}
#endif //FLOOR_H
