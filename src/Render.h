//
// Created by tuxic on 21/12/2023.
//

#ifndef RENDER_H
#define RENDER_H
#include <cmath>
#include "Config.h"

struct CameraData {
    CameraData(const sf::Vector2<double> &position, const sf::Vector2<double> &direction,
        const sf::Vector2<double> &plane)
        : position(position),
          direction(direction),
          plane(plane) {
    }
    sf::Vector2<double> position;
    sf::Vector2<double> direction;
    sf::Vector2<double> plane;
    double positionZ = 0;
    double pitch = 0;
};

struct RenderData {
    sf::Uint8 *buffer;
    std::vector<sf::Image *> textures;

    void clearBuffer() const {
        for (int x = 0; x < RENDER_WIDTH; x++) {
            for (int y = 0; y < RENDER_HEIGHT; y++) {
                setColor(x, y, CLEAR_COLOR);
            }
        }
    }

    void setColor(const int x, const int y, const sf::Color color) const {
        buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS] = color.r;
        buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 1] = color.g;
        buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 2] = color.b;
        buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 3] = color.a;
    }
};

struct RaycastData {
    sf::Vector2<double> rayDirection;
    sf::Vector2i mapPosition;
    double wallDistance{};
    double wallX{};
    int side{};
    int lineHeight{};
    int drawStart{};
    int drawEnd{};
    int textureId{};
    int textureX{};
    double step{};
    double texturePosition{};
    sf::Vector2<double> floorWall;
    int wallHeight = 3;

    void populateTextureParameters(const sf::Vector2<double> &position, const double &positionZ, const double &pitch) {
        textureId = worldMap[mapPosition.x][mapPosition.y] - 1;

        //calculate value of wallX
        if (side % 2 == 0) wallX = position.y + wallDistance * rayDirection.y;
        else wallX = position.x + wallDistance * rayDirection.x;
        wallX -= floor((wallX));

        textureX = static_cast<int>(wallX * static_cast<double>(TEX_WIDTH));
        if (side % 2 == 0 && rayDirection.x > 0) textureX = TEX_WIDTH - textureX - 1;
        if (side % 2 == 1 && rayDirection.y < 0) textureX = TEX_WIDTH - textureX - 1;
        step = 1.0 * TEX_HEIGHT / lineHeight;
        texturePosition = (drawStart - pitch - positionZ - RENDER_HEIGHT / 2 + lineHeight / 2) * step;
    }
};

inline void applyFog(const double distance, sf::Color &color) {
    double delta = std::clamp(distance / FOG_DISTANCE, 0.0, 1.0);
    delta *= delta * delta;
    color.r = std::lerp(color.r, CLEAR_COLOR.r, delta);
    color.g = std::lerp(color.g, CLEAR_COLOR.g, delta);
    color.b = std::lerp(color.b, CLEAR_COLOR.b, delta);
}

inline void drawColumn(const int x, RaycastData &raycast, const RenderData *renderData) {
    if (raycast.wallDistance > FOG_DISTANCE) return;
    for (int y = raycast.drawStart; y <= raycast.drawEnd; ++y) {
        // Cast the texture coordinate to integer, and mask with (TEX_HEIGHT - 1) in case of overflow
        const int texY = static_cast<int>(raycast.texturePosition) & (TEX_HEIGHT - 1);
        raycast.texturePosition += raycast.step;
        auto color = renderData->textures[raycast.textureId]->getPixel(raycast.textureX, texY);
        applyFog(raycast.wallDistance, color);
        renderData->setColor(x, y, color);
    }
}

inline void drawFloorWall(const int x, RaycastData &raycast,
                          const CameraData *camera, const RenderData *renderData) {
    if (raycast.drawEnd < 0) raycast.drawEnd = RENDER_HEIGHT; //becomes < 0 when the integer overflows

    // ceiling
    for (int y = raycast.drawStart - 1; y >= 0; y--) {
        constexpr double distPlayer = 0.0;

        const double ceilingHeight = 2.0 * (raycast.wallHeight - 1) * RENDER_HEIGHT;
        const double currentDist = (RENDER_HEIGHT - 2.0 * camera->positionZ + ceilingHeight) / (RENDER_HEIGHT - 2.0 * (y - camera->pitch));
        //you could make a small lookup table for this instead

        const double weight = (currentDist - distPlayer) / (raycast.wallDistance - distPlayer);

        const double currentFloorX = weight * raycast.floorWall.x + (1.0 - weight) * camera->position.x;
        const double currentFloorY = weight * raycast.floorWall.y + (1.0 - weight) * camera->position.y;

        const int floorTexX = std::clamp(static_cast<int>(currentFloorX * TEX_WIDTH) % TEX_WIDTH, 0, TEX_WIDTH);
        const int floorTexY = std::clamp(static_cast<int>(currentFloorY * TEX_HEIGHT) % TEX_HEIGHT, 0, TEX_HEIGHT);

        auto color = renderData->textures[CEILING_TEXTURE]->getPixel(floorTexX, floorTexY);
        applyFog(currentDist, color);
        renderData->setColor(x, y, color);
    }

    // floor
    for (int y = raycast.drawEnd + 1; y < RENDER_HEIGHT; y++) {
        constexpr double distPlayer = 0.0;
        const double currentDist = (RENDER_HEIGHT + 2.0 * camera->positionZ) / (2.0 * (y - camera->pitch) - RENDER_HEIGHT);
        //you could make a small lookup table for this instead

        const double weight = (currentDist - distPlayer) / (raycast.wallDistance - distPlayer);

        const double currentFloorX = weight * raycast.floorWall.x + (1.0 - weight) * camera->position.x;
        const double currentFloorY = weight * raycast.floorWall.y + (1.0 - weight) * camera->position.y;

        const int floorTexX = std::clamp(static_cast<int>(currentFloorX * TEX_WIDTH) % TEX_WIDTH, 0, TEX_WIDTH);
        const int floorTexY = std::clamp(static_cast<int>(currentFloorY * TEX_HEIGHT) % TEX_HEIGHT, 0, TEX_HEIGHT);

        auto color = renderData->textures[FLOOR_TEXTURE]->getPixel(floorTexX, floorTexY);
        applyFog(currentDist, color);
        renderData->setColor(x, y, color);
    }
}

#endif //RENDER_H
