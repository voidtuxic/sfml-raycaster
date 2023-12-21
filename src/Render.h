//
// Created by tuxic on 21/12/2023.
//

#ifndef RENDER_H
#define RENDER_H
#include "Config.h"

inline void setColor(sf::Uint8* buffer, const int x, const int y, const sf::Color color) {
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS] = color.r;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 1] = color.g;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 2] = color.b;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 3] = color.a;
}

inline void clearBuffer(sf::Uint8* buffer) {
    for (int x = 0; x < RENDER_WIDTH; x++) {
        for (int y = 0; y < RENDER_HEIGHT; y++) {
            setColor(buffer, x, y, CLEAR_COLOR);
        }
    }
}

inline void applyFog(const double distance, sf::Color &color) {
    double delta = std::clamp(distance / FOG_DISTANCE, 0.0, 1.0);
    delta *= delta * delta;
    color.r = std::lerp(color.r, CLEAR_COLOR.r, delta);
    color.g = std::lerp(color.g, CLEAR_COLOR.g, delta);
    color.b = std::lerp(color.b, CLEAR_COLOR.b, delta);
}

inline void getTextureParameters(const sf::Vector2<double> &position, const sf::Vector2<double> &rayDir,
                                 const sf::Vector2i &map, const double perpWallDist, const int side,
                                 const int lineHeight, const int drawStart, int &texNum, int &texX, double &step,
                                 double &texPos, double &wallX) {
    texNum = worldMap[map.x][map.y] - 1;

    //calculate value of wallX
    if (side % 2 == 0) wallX = position.y + perpWallDist * rayDir.y;
    else wallX = position.x + perpWallDist * rayDir.x;
    wallX -= floor((wallX));

    texX = static_cast<int>(wallX * static_cast<double>(TEX_WIDTH));
    if (side % 2 == 0 && rayDir.x > 0) texX = TEX_WIDTH - texX - 1;
    if (side % 2 == 1 && rayDir.y < 0) texX = TEX_WIDTH - texX - 1;
    step = 1.0 * TEX_HEIGHT / lineHeight;
    texPos = (drawStart - RENDER_HEIGHT / 2 + lineHeight / 2) * step;
}

inline void drawColumn(const int x, const int drawStart, const int drawEnd, const int texNum, const int texX,
                       const double step, double texPos, const double perpWallDist, sf::Uint8 *buffer,
                       const std::vector<sf::Image *> &textures) {
    if(perpWallDist > FOG_DISTANCE) return;
    for (int y = drawStart; y <= drawEnd; ++y) {
        // Cast the texture coordinate to integer, and mask with (TEX_HEIGHT - 1) in case of overflow
        const int texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);
        texPos += step;
        auto color = textures[texNum]->getPixel(texX, texY);
        applyFog(perpWallDist, color);
        setColor(buffer, x, y, color);
    }
}

inline void drawFloorWall(const sf::Vector2<double> &position, const int x, const double perpWallDist, int &drawEnd,
                          const sf::Vector2<double> floorWall, sf::Uint8 *buffer,
                          const std::vector<sf::Image *> &textures) {
    if (drawEnd < 0) drawEnd = RENDER_HEIGHT; //becomes < 0 when the integer overflows

    //draw the floor from drawEnd to the bottom of the screen
    for(int y = drawEnd + 1; y < RENDER_HEIGHT; y++)
    {
        constexpr double distPlayer = 0.0;
        const double currentDist = RENDER_HEIGHT / (2.0 * y - RENDER_HEIGHT); //you could make a small lookup table for this instead

        const double weight = (currentDist - distPlayer) / (perpWallDist - distPlayer);

        const double currentFloorX = weight * floorWall.x + (1.0 - weight) * position.x;
        const double currentFloorY = weight * floorWall.y + (1.0 - weight) * position.y;

        const int floorTexX = static_cast<int>(currentFloorX * TEX_WIDTH) % TEX_WIDTH;
        const int floorTexY = static_cast<int>(currentFloorY * TEX_HEIGHT) % TEX_HEIGHT;

        //floor
        auto color = textures[FLOOR_TEXTURE]->getPixel(floorTexX, floorTexY);
        applyFog(currentDist, color);
        setColor(buffer, x, y, color);
        //ceiling (symmetrical!)
        color = textures[CEILING_TEXTURE]->getPixel(floorTexX, floorTexY);
        applyFog(currentDist, color);
        setColor(buffer, x, RENDER_HEIGHT - y, color);
    }
}

#endif //RENDER_H
