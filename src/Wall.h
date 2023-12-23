//
// Created by tuxic on 21/12/2023.
//

#ifndef WALL_H
#define WALL_H
#include <SFML/Graphics.hpp>
#include "Config.h"

namespace wolf {
    inline void calculateStep(const glm::vec2 &position, const glm::vec2 &rayDir,
                              const glm::vec2 &deltaDist, const glm::ivec2 &mapPosition, glm::vec2 &sideDist,
                              glm::ivec2 &step) {
        //what direction to step in x or y-direction (either +1 or -1)
        if (rayDir.x < 0) {
            step.x = -1;
            sideDist.x = (position.x - static_cast<float>(mapPosition.x)) * deltaDist.x;
        } else {
            step.x = 1;
            sideDist.x = (static_cast<float>(mapPosition.x) + 1.0f - position.x) * deltaDist.x;
        }
        if (rayDir.y < 0) {
            step.y = -1;
            sideDist.y = (position.y - static_cast<float>(mapPosition.y)) * deltaDist.y;
        } else {
            step.y = 1;
            sideDist.y = (static_cast<float>(mapPosition.y) + 1.0f - position.y) * deltaDist.y;
        }
    }

    inline void performDDA(const int map[MAP_WIDTH][MAP_HEIGHT], int &side, const glm::vec2 &deltaDist,
                           const glm::ivec2 &step, glm::ivec2 &mapPosition, glm::vec2 &sideDist) {
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

    inline void calculateWall(const int x, RaycastData &raycast, const int map[MAP_WIDTH][MAP_HEIGHT], const CameraData* camera) {
        //calculate ray position and direction
        const double cameraX = 2 * x / static_cast<double>(RENDER_WIDTH) - 1; //x-coordinate in camera space
        raycast.rayDirection = glm::vec2(camera->direction.x + camera->plane.x * cameraX, camera->direction.y + camera->plane.y * cameraX);
        raycast.mapPosition = glm::ivec2(static_cast<int>(camera->position.x), static_cast<int>(camera->position.y));

        //length of ray from current position to next x or y-side
        glm::vec2 sideDist;
        glm::ivec2 step;

        //length of ray from one x or y-side to next x or y-side
        const glm::vec2 deltaDist = glm::vec2(
            (raycast.rayDirection.x == 0) ? 1e30 : std::abs(1 / raycast.rayDirection.x),
            (raycast.rayDirection.y == 0) ? 1e30 : std::abs(1 / raycast.rayDirection.y));

        //calculate step and initial sideDist
        calculateStep(camera->position, raycast.rayDirection, deltaDist, raycast.mapPosition, sideDist, step);

        //perform DDA
        performDDA(map, raycast.side, deltaDist, step, raycast.mapPosition, sideDist);

        //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
        if (raycast.side % 2 == 0) raycast.wallDistance = (sideDist.x - deltaDist.x);
        else raycast.wallDistance = (sideDist.y - deltaDist.y);
        raycast.lineHeight = static_cast<int>(RENDER_HEIGHT / raycast.wallDistance);

        const auto wallHeight = raycast.wallHeight;
        raycast.drawStart = -raycast.lineHeight * (0.5 + wallHeight - 1) + RENDER_HEIGHT / 2 + camera->pitch + camera->positionZ / raycast.wallDistance;
        if (raycast.drawStart < 0) raycast.drawStart = 0;
        raycast.drawEnd = raycast.lineHeight / 2 + RENDER_HEIGHT / 2  + camera->pitch + camera->positionZ / raycast.wallDistance;
        if (raycast.drawEnd >= RENDER_HEIGHT) raycast.drawEnd = RENDER_HEIGHT - 1;
    }
}

#endif //WALL_H
